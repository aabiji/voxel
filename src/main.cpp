#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include "shader.h"
#include "engine.h"

void resize_callback(GLFWwindow* window, int width, int height)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->handle_resize(width, height);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->handle_mouse_move(xpos, ypos);
}

void handle_keyboard_input(GLFWwindow* window, Engine& engine)
{
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        engine.move_player(Direction::down);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        engine.move_player(Direction::up);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        engine.move_player(Direction::left);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        engine.move_player(Direction::right);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        engine.move_player(Direction::front);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        engine.move_player(Direction::back);
}

int main()
{
    if (!glfwInit())
        log(Level::fatal, "Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(900, 700, "Voxel", NULL, NULL);
    if (!window)
        log(Level::fatal, "Failed to create window");

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetWindowSizeCallback(window, resize_callback);
    glfwSwapInterval(1);

    {
        Engine engine;
        glfwSetWindowUserPointer(window, &engine);

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            handle_keyboard_input(window, engine);
            engine.render();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
