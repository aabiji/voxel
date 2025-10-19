#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "camera.h"
#include "spritesheet.h"

void cleanup_opengl();

class Engine
{
public:
    Engine();
    ~Engine();
    void run();
private:
    static int m_window_width;
    static int m_window_height;
    float m_delta_time;
    float m_last_frame;

    GLFWwindow* m_window;
    Camera m_camera;
    Shader m_shader;
    Spritesheet m_block_textures;

    void init_context();
    void init_systems();

    void handle_keyboard_input();
    static void handle_opengl_error(
        GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam);
    static void handle_window_size(GLFWwindow* window, int width, int height);
    static void handle_mouse_move(GLFWwindow* window, double x, double y);
    static void handle_mouse_clicks(GLFWwindow* window, int button, int action, int mods);
};
