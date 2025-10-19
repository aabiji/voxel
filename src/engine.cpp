#include "engine.h"
#include "utils.h"

const float cube_vertices[] = {
    // front face
   -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,

    // back face
   -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,

    // top face
   -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
   -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
   -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,

    // bottom face
   -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 2.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 2.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 2.0f,
   -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 2.0f,
   -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 2.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 2.0f,

    // right face
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

    // left face
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f
};

int Engine::m_window_width = 900;
int Engine::m_window_height = 900;

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

    m_delta_time = 0;
    m_last_frame = 0;
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
    log("OPENGL", message, ltype);
}


void Engine::handle_window_size(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    m_window_width = width;
    m_window_height = height;
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
        m_camera.move(m_delta_time, Direction::FORWARD);

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

void Engine::handle_mouse_clicks(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    (void)mods;
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
    unsigned int vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    int stride = 6 * sizeof(float);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinate
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::array<Matrix<4, 4>, 10> models;
    for (int i = 0; i < 10; i++) {
        float angle = random(0, 90);
        Vec<3> axis = {
            random(0, 1), random(0, 1), random(0, 1)
        };

        auto translation = Matrix<4, 4>::from_translation(
            random(-5, 5), random(-2, 2), -random(8, 12));

        auto n = random(0.5, 1);
        auto scale = Matrix<4, 4>::from_scale(n, n, n);

        Quaternion q(radians(angle), axis);
        models[i] = translation * q.to_matrix() * scale;
    }

    while (!glfwWindowShouldClose(m_window)) {
        float current_frame = glfwGetTime();
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;
        handle_keyboard_input();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader.use();

        Matrix<4, 4> projection = PerspectiveProjection(
            0.1f, 100.0f, (float)m_window_width / (float)m_window_height, radians(45));
        m_shader.set_matrix("projection", projection);

        Matrix<4, 4> view = m_camera.view_matrix();
        m_shader.set_matrix("view", view);

        glBindVertexArray(vao);
        m_block_textures.bind();
        for (size_t i = 0; i < models.size(); i++) {
            m_shader.set_matrix("model", models[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
