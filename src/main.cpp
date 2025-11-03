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

void keybinding_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        GLint polygon_mode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
        if (polygon_mode[0] == GL_FILL)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
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

void debug_callback(GLenum source, GLenum type, unsigned int id,
                    GLenum severity, GLsizei length, const char *message,
                    const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string source_info = "";
    switch (source) {
        case GL_DEBUG_SOURCE_API:             source_info += "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_info += "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_info += "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_info += "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_info += "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           source_info += "Source: Other"; break;
    }

    std::string type_info = "";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               type_info += "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_info += "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_info += "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_info += "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_info += "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              type_info += "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_info += "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_info += "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               type_info += "Type: Other"; break;
    }

    Level level = severity == GL_DEBUG_SEVERITY_HIGH
        ? Level::error
        : severity == GL_DEBUG_SEVERITY_MEDIUM ? Level::warning : Level::info;
    log(level, "{} {} {}", source_info, type_info, message);
}

int main()
{
    if (!glfwInit())
        log(Level::fatal, "Failed to init GLFW");

    int width = 900, height = 700;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Voxel", NULL, NULL);
    if (!window)
        log(Level::fatal, "Failed to create window");

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetWindowSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, keybinding_callback);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, double(width) / 2, double(height) / 2);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    {
        Engine engine(width, height);
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
