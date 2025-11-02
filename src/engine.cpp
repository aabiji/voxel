#include <glad/glad.h>

#include "engine.h"

Engine::Engine()
{
    m_window_width = 900;
    m_window_height = 700;

    float aspect = float(m_window_width) / float(m_window_height);
    m_projection = Matrix4::projection(0.1, 100.0, 45 * (M_PI / 180), aspect);
    m_view = m_camera.look_at();

    auto result = m_shaders.add_shader(GL_VERTEX_SHADER, "assets/shaders/vertex.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    result = m_shaders.add_shader(GL_FRAGMENT_SHADER, "assets/shaders/fragment.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    result = m_shaders.assemble();
    if (result.is_err())
        log(Level::fatal, result.error());

    m_spritesheet.load("assets/textures/atlas.png", 64, 3);
}

void Engine::handle_mouse_move(float x, float y)
{
    m_camera.rotate(x, y);
    m_view = m_camera.look_at();
}

void Engine::move_player(Direction direction)
{
    m_camera.move(direction);
    m_view = m_camera.look_at();
}

void Engine::handle_resize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_window_width = width;
    m_window_height = height;

    float aspect = float(width) / float(height);
    m_projection = Matrix4::projection(0.1, 1.0, 45 * (M_PI / 180), aspect);
}

void Engine::render()
{
    m_shaders.use();
    m_shaders.set_matrix4("projection", m_projection);
    m_shaders.set_matrix4("view", m_view);
    m_spritesheet.bind(m_shaders, 0);
    m_chunk.render(m_shaders);
}