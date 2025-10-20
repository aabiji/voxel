#include "chunk.h"
#include "engine.h"
#include "utils.h"

/*
todo:
- debug our chunk rendering (get it to work)
- provide move constructors for the classes
- use std::expected instead of throwing exceptions
- call glfwTerminate in Engine's deconstructor
- use std::ostreamstream in the to_string() methods
*/

Engine::Engine()
{
    init_context();
    init_systems();
}

void cleanup_opengl() { glfwTerminate(); }

Engine::~Engine() { glfwDestroyWindow(m_window); }

void Engine::init_context()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window_width = 900, m_window_height = 700;
    m_window = glfwCreateWindow(m_window_width, m_window_height,
                                "Voxel", nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("Failed to create window");

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, Engine::handle_window_size);
    glfwSetCursorPosCallback(m_window, Engine::handle_mouse_move);
    glfwSetMouseButtonCallback(m_window, Engine::handle_mouse_clicks);
    glfwSetCursorPos(m_window, m_window_width / 2.0, m_window_height / 2.0);

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(Engine::handle_opengl_error, 0);
}

void Engine::init_systems()
{
    m_shader.add(GL_VERTEX_SHADER, "assets/shaders/vertex.glsl");
    m_shader.add(GL_FRAGMENT_SHADER, "assets/shaders/fragment.glsl");
    m_shader.assemble();

    m_block_textures.load("assets/textures/atlas.png", 3, 64);
    m_block_textures.set_unit(m_shader, "textures", 0);

    m_camera.set_initial_mouse_position(m_window_width / 2.0, m_window_height / 2.0);

    m_delta_time = 0;
    m_last_frame = 0;
}

void Engine::handle_opengl_error(
    [[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type,
    [[maybe_unused]] GLuint id, GLenum severity,
    [[maybe_unused]] GLsizei length, const GLchar* message,
    [[maybe_unused]] const void* userParam)
{
    Logtype ltype = severity == GL_DEBUG_SEVERITY_HIGH ? Logtype::ERROR
        : severity == GL_DEBUG_SEVERITY_MEDIUM ? Logtype::WARNING
        : Logtype::INFO;
    log("OPENGL", message, ltype);
}

void Engine::resize(int width, int height)
{
    m_window_width = width;
    m_window_height = height;
}

void Engine::handle_window_size(GLFWwindow* window, int width, int height)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->resize(width, height);
    glViewport(0, 0, width, height);
}

void Engine::handle_keyboard_input()
{
    // toggle wireframe
    if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        GLint next = polygonMode[0] == GL_LINE ? GL_FILL : GL_LINE;
        glPolygonMode(GL_FRONT_AND_BACK, next);
    }

    // camera movement
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::FRONT);

    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::BACK);

    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::UP);

    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::DOWN);

    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::LEFT);

    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::RIGHT);
}

void Engine::handle_mouse_clicks(
    [[maybe_unused]]GLFWwindow* window,
    int button, int action, [[maybe_unused]]int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        log("INFO", "Right click");
}

void Engine::handle_mouse_move(GLFWwindow* window, double x, double y)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->m_camera.rotate(x, y);
}

void Engine::run()
{
    Chunk chunk;

    while (!glfwWindowShouldClose(m_window)) {
        float current_frame = glfwGetTime();
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;
        handle_keyboard_input();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader.use();

        Matrix<4, 4> projection = perspective_projection(
            0.1f, 100.0f, (float)m_window_width / (float)m_window_height, radians(45));
        m_shader.set_matrix("projection", projection);

        Matrix<4, 4> view = m_camera.view_matrix();
        m_shader.set_matrix("view", view);

        m_block_textures.bind();
        chunk.draw();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}
