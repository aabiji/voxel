#include <stdexcept>
#include <string>
#include <iostream>

// TODO: fix camera movement (handle key presses in main loop, not callback)
// TODO: proper camera translation
// TODO: add cmaera rotation using mouse
// TODO: now think about rendering multiple cubes in a lattice

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "math.h"

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

enum class Logtype { ERROR, WARNING, INFO };

void log(std::string tag, Logtype type, std::string message)
{
    int color = type == Logtype::ERROR ? 31 : type == Logtype::WARNING ? 33 : 36;
    std::cout << std::format("\x1b[1;{}m[{}]: {}\u001b[0m\n", color, tag, message);
}

class Texture
{
public:
    Texture(const char* path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    unsigned int id() { return m_texture; }
private:
    int m_width, m_height;
    unsigned int m_texture;
};

Texture::~Texture() { glDeleteTextures(1, &m_texture); }

Texture::Texture(const char* path)
{
    int channels; // requesting it to always be 4
    unsigned char* pixels = stbi_load(path, &m_width, &m_height, &channels, 4);
    if (pixels == nullptr)
        throw std::runtime_error("Failed to open " + std::string(path));

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    stbi_image_free(pixels);
}

class Camera
{
public:
    Camera()
        : m_position(0.0, 0.0, 5.0),
        m_front_direction(0.0, 0.0, -1.0) {}

    math::Matrix<4, 4> view_matrix()
    {
        const auto up = math::Vec<3>(0.0, 1.0, 0.0);
        const auto p = m_position + m_translation;
        return math::LookAt(p, p + m_front_direction, up);
    }

    void move(math::Vec<3> direction)
    {
        m_translation = m_translation + direction;
        log("CAMERA", Logtype::INFO, std::format("{}, {}, {}", m_translation.arr[0], m_translation.arr[1], m_translation.arr[2]));
    }

private:
    math::Vec<3> m_position;
    math::Vec<3> m_front_direction;
    math::Vec<3> m_translation;
};

class Engine
{
public:
    Engine();
    ~Engine();

    void run();
private:
    static int m_window_width;
    static int m_window_height;
    GLFWwindow* m_window;

    Camera m_camera;

    void init_window();
    void init_context();

    static void handle_opengl_error(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void handle_window_size(GLFWwindow* window, int width, int height);
    static void handle_mouse_move(GLFWwindow* window, double x, double y);
    static void handle_mouse_click(GLFWwindow* window, int button, int action, int mods);
    static void handle_keyboard_input(
        GLFWwindow* window, int key, int scancode, int action, int mods);
};

int Engine::m_window_width = 900;
int Engine::m_window_height = 900;

Engine::Engine()
{
    init_window();
    init_context();
}

Engine::~Engine()
{
    if (m_window)
        glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::init_window()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_window_width, m_window_height,
                                "Voxel", nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("Failed to create window");

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, Engine::handle_keyboard_input);
    glfwSetWindowSizeCallback(m_window, Engine::handle_window_size);
    glfwSetCursorPosCallback(m_window, Engine::handle_mouse_move);
    glfwSetMouseButtonCallback(m_window, Engine::handle_mouse_click);
}

void Engine::init_context()
{
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Engine::handle_opengl_error, 0);
}

void Engine::handle_opengl_error(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    (void)source;
    (void)type;
    (void)id;
    (void)userParam;
    (void)length;
    Logtype ltype = severity == GL_DEBUG_SEVERITY_HIGH ? Logtype::ERROR
        : severity == GL_DEBUG_SEVERITY_MEDIUM ? Logtype::WARNING
        : Logtype::INFO;
    log("OPENGL", ltype, message);
}


void Engine::handle_window_size(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    m_window_width = width;
    m_window_height = height;
}

void Engine::handle_keyboard_input(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Engine* ptr = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // toggle wireframe
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        GLint next = polygonMode[0] == GL_LINE ? GL_FILL : GL_LINE;
        glPolygonMode(GL_FRONT_AND_BACK, next);
    }

    if (key == GLFW_KEY_W && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(0, 0, -1));

    if (key == GLFW_KEY_S && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(0, 0, 1));

    if (key == GLFW_KEY_A && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(-1, 0, 0));

    if (key == GLFW_KEY_D && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(1, 0, 0));

    if (key == GLFW_KEY_UP && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(0, 1, 0));

    if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
        ptr->m_camera.move(math::Vec<3>(-0 -1, 0));
}

void Engine::handle_mouse_click(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        log("INFO", Logtype::INFO, "Right click");
}

void Engine::handle_mouse_move(GLFWwindow* window, double x, double y)
{
    (void)window;
    std::string msg = std::format("Mouse move: ({}, {})", x, y);
    log("INFO", Logtype::INFO, msg.c_str());
}

void Engine::run()
{
    Shader shader;
    shader.add(GL_VERTEX_SHADER, "../assets/shaders/vertex.glsl");
    shader.add(GL_FRAGMENT_SHADER, "../assets/shaders/fragment.glsl");
    shader.assemble();

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

    // create texture and set the texture unit
    Texture texture("../assets/textures/image.png");
    shader.use();
    shader.set_int("texture1", 0);

    auto translation = math::Matrix<4, 4>::from_translation(0, 0, -10);
    auto scale = math::Matrix<4, 4>::from_scale(1, 1, 1);

    math::Quaternion q(math::radians(45), { 0, 1, 0 });
    q = q * math::Quaternion(math::radians(45), { 1, 0, 0 });
    auto rotation = q.to_matrix();

    auto model = translation * rotation * scale;

    while (!glfwWindowShouldClose(m_window)) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        math::Matrix<4, 4> projection = math::PerspectiveProjection(
            0.1f, 100.0f, (float)m_window_width / (float)m_window_height, math::radians(45));
        shader.set_matrix("projection", projection);

        math::Matrix<4, 4> view = m_camera.view_matrix();
        shader.set_matrix("view", view);

        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.id());

        shader.set_matrix("model", model);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices) / stride);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

int main()
{
    try {
        Engine engine{};
        engine.run();
    } catch (const std::runtime_error& err) {
        log("ENGINE", Logtype::ERROR, err.what());
        return -1;
    }
    return 0;
}
