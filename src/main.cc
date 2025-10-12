#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

// TODO: textures!
// TODO: control the camera
// TODO: add an EBO to our cube vertices (how should we draw cubes btw??)
// TODO: refactor (really think about it)

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::string;
using std::vector;

const float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

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
    void use() { glUseProgram(m_program); }
    void cleanup() { glDeleteProgram(m_program); }

    // all calls to add must be followed by assemble
    void assemble();
    void add(unsigned int type, string path);

    // set uniforms
    void set_int(const char* name, int value) const;
    void set_matrix(const char* name, glm::mat4& value) const;
private:
    unsigned int m_program;
    vector<unsigned int> m_shaders;
};

void Shader::set_int(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name), value);
}

void Shader::set_matrix(const char* name, glm::mat4& value) const
{
    int location = glGetUniformLocation(m_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

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

    // toggle wireframe
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        GLint next = polygonMode[0] == GL_LINE ? GL_FILL : GL_LINE;
        glPolygonMode(GL_FRONT_AND_BACK, next);
    }
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader shader;
    try {
        shader.add(GL_VERTEX_SHADER, "../assets/shaders/vertex.glsl");
        shader.add(GL_FRAGMENT_SHADER, "../assets/shaders/fragment.glsl");
        shader.assemble();
    } catch (const std::runtime_error& err) {
        log("SHADER", ERROR, err.what());
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    int stride = 5 * sizeof(float);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::vec3 camera_pos = glm::vec3(0.0, 0.0, 3.0);
    glm::vec3 camera_target = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 camera_up = glm::vec3(0.0, 1.0, 0.0);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::mat4(1.0);
        projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height,
            0.1f, 100.0f);
        shader.set_matrix("projection", projection);

        glm::mat4 view = glm::mat4(1.0);
        view = glm::lookAt(camera_pos, camera_target, camera_up);
        shader.set_matrix("view", view);

        glBindVertexArray(vao);
        glm::mat4 model = glm::mat4(1.0);
        shader.set_matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices) / stride);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    shader.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
