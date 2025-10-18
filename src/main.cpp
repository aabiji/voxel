#include <stdexcept>
#include <iostream>

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

enum class Direction { FORWARD, BACK, LEFT, RIGHT };

class Camera
{
public:
    Camera()
        : m_position(0.0, 0.0, 5.0),
        m_front_direction(0.0, 0.0, -1.0),
        m_up(0.0, 1.0, 0.0),
        m_orientation(1.0, 0.0, 0.0, 0.0),
        m_sensitivity(1),
        m_pitch(0), m_yaw(0) {}

    math::Matrix<4, 4> view_matrix()
    {
        return math::LookAt(m_position, m_position + m_front_direction, m_up);
    }

    void rotate(float deltaX, float deltaY)
    {
        m_yaw += deltaX * m_sensitivity;
        m_pitch += deltaY * m_sensitivity;

        math::Vec<3> direction = (m_position - (m_position + m_front_direction)).normalize();
        math::Vec<3> right = cross(direction, m_up).normalize();
        math::Quaternion yaw_quat(m_yaw, m_up);
        math::Quaternion pitch_quat(m_pitch, right);

        m_orientation = yaw_quat * pitch_quat * m_orientation;
    }

    void move(float delta_time, Direction d)
    {
        float speed = 2 * delta_time;
        auto z = m_front_direction * speed;
        auto x = math::cross(m_front_direction, m_up).normalize() * speed;
        if (d == Direction::FORWARD) m_position += z;
        if (d == Direction::BACK) m_position -= z;
        if (d == Direction::RIGHT) m_position -= x;
        if (d == Direction::LEFT) m_position += x;
    }

private:
    math::Vec<3> m_position;
    math::Vec<3> m_front_direction;
    math::Vec<3> m_up;

    math::Quaternion m_orientation;
    float m_sensitivity;
    float m_pitch, m_yaw;
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

    float m_delta_time;
    float m_last_frame;
    float m_previous_x;
    float m_previous_y;

    void init_window();
    void init_context();

    void handle_keyboard_input();
    void handle_mouse_move();
    void handle_mouse_clicks();

    static void handle_opengl_error(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void handle_window_size(GLFWwindow* window, int width, int height);
};

int Engine::m_window_width = 900;
int Engine::m_window_height = 900;

Engine::Engine()
{
    init_window();
    init_context();

    m_delta_time = 0;
    m_last_frame = 0;
    m_previous_x = 0;
    m_previous_y = 0;
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
    glfwSetWindowSizeCallback(m_window, Engine::handle_window_size);
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

void Engine::handle_keyboard_input()
{
    // toggle wireframe
    if (glfwGetKey(m_window, GLFW_KEY_M)) {
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
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::LEFT);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.move(m_delta_time, Direction::RIGHT);
}

void Engine::handle_mouse_clicks()
{
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        log("INFO", Logtype::INFO, "Right click");
}

void Engine::handle_mouse_move()
{
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    if (x != 0 || y != 0) {
        // delta y's reversed since (0, 0) starts at the
        // bottom left corner in opengl, not the top left
        m_camera.rotate(float(x) - m_previous_x, m_previous_y - float(y));
        m_previous_x = x;
        m_previous_y = y;
    }
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

    std::array<math::Matrix<4, 4>, 10> models;
    for (int i = 0; i < 10; i++) {
        float angle = math::random(0, 90);
        math::Vec<3> axis = {
            math::random(0, 1), math::random(0, 1), math::random(0, 1)
        };

        auto translation = math::Matrix<4, 4>::from_translation(
            math::random(-5, 5), math::random(-2, 2), -math::random(8, 12));

        auto n = math::random(0.5, 1);
        auto scale = math::Matrix<4, 4>::from_scale(n, n, n);

        math::Quaternion q(math::radians(angle), axis);
        models[i] = translation * q.to_matrix() * scale;
    }

    while (!glfwWindowShouldClose(m_window)) {
        float current_frame = glfwGetTime();
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;

        handle_keyboard_input();
        handle_mouse_clicks();
        handle_mouse_move();

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
        for (size_t i = 0; i < models.size(); i++) {
            shader.set_matrix("model", models[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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
