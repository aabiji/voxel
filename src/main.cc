#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>

using std::string;
using std::vector;

enum LogType { ERROR, WARNING, INFO };
void log(string tag, LogType type, string message)
{
    // fatal = red, warning = yellow, info = cyan
    int color = type == ERROR ? 31 : type == WARNING ? 33 : 36;
    string reset = "\u001b[0m";
    std::cout << "\x1b[1;" << color << "m";
    std::cout << "[" << tag << "]: " << message << reset << "\n";
}

class Shader
{
public:
    // all calls to add must be followed by assemble
    void assemble();
    void add(unsigned int type, string path);

    unsigned int program() { return m_program; }
    void cleanup() { glDeleteProgram(m_program); }
private:
    unsigned int m_program;
    vector<unsigned int> m_shaders;
};

void Shader::add(unsigned int type, string path)
{
    // read the file
    std::ifstream file(path);
    if (!file.is_open() || !file.good())
        throw std::runtime_error(std::format("Failed to open {}", path));

    string source = "", line = "";
    while (std::getline(file, line))
        source += line + '\n'; 
    file.close();

    // compile the shader
    const char* str = source.c_str();
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &str, nullptr);
    glCompileShader(shader);

    // check for errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        throw std::runtime_error(string(info));
    }

    m_shaders.push_back(shader);
}

void Shader::assemble()
{
    // assemble and link the program
    m_program = glCreateProgram();
    for (size_t i = 0; i < m_shaders.size(); i++)
        glAttachShader(m_program, m_shaders[i]);
    glLinkProgram(m_program);

    // check for errors
    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_program, 512, nullptr, info);
        throw std::runtime_error(string(info));
    }

    // cleanup
    int size = m_shaders.size();
    for (int i = 0; i < size; i++) {
        glDeleteShader(m_shaders[m_shaders.size() - 1]);
        m_shaders.pop_back();
    }
}

static void error_callback([[maybe_unused]] int error,
                           const char* description)
{
    log("GLFW", ERROR, std::string(description));
}

static void key_callback(GLFWwindow* window, int key,
                         [[maybe_unused]] int scancode,
                         [[maybe_unused]] int action,
                         [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(900, 700, "Voxel", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    Shader shader;
    try {
        shader.add(GL_VERTEX_SHADER, "../shaders/vertex.glsl");
        shader.add(GL_FRAGMENT_SHADER, "../shaders/fragment.glsl");
        shader.assemble();
    } catch (const std::runtime_error& err) {
        log("SHADER", ERROR, err.what());
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(1.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
