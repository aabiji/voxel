#include <glad/glad.h>

#include "engine.h"

Engine::Engine()
{
    m_window_width = 900;
    m_window_height = 700;

    auto result = m_shaders.add_shader(GL_VERTEX_SHADER, "assets/shaders/vertex.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    result = m_shaders.add_shader(GL_FRAGMENT_SHADER, "assets/shaders/fragment.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    result = m_shaders.assemble();
    if (result.is_err())
        log(Level::fatal, result.error());
}

void Engine::handle_resize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_window_width = width;
    m_window_height = height;
}

void Engine::handle_mouse_move(int x, int y)
{
}

void Engine::render()
{
    m_shaders.use();
}

void Engine::move_player(Direction direction)
{

}