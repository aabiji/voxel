#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "camera.h"
#include "shader.h"

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

    void init_window();
    void init_context();

    void handle_keyboard_input();
    static void handle_opengl_error(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void handle_window_size(GLFWwindow* window, int width, int height);
    static void handle_mouse_move(GLFWwindow* window, double x, double y);
    static void handle_mouse_clicks(GLFWwindow* window, int button, int action, int mods);
};
