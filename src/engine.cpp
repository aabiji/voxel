#include <glad/glad.h>

#include "engine.h"

Engine::Engine(int window_width, int window_height)
{
    float aspect = float(window_width) / float(window_height);
    m_projection = Matrix4::projection(0.1, 100.0, 45 * (M_PI / 180), aspect);
    m_view = m_camera.look_at();

    m_spritesheet.load("assets/textures/atlas.png", 64, 3);

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
    float aspect = float(width) / float(height);
    m_projection = Matrix4::projection(0.1, 100.0, 45 * (M_PI / 180), aspect);
}

void Engine::render()
{
    m_shaders.use();
    m_shaders.set_matrix4("projection", m_projection);
    m_shaders.set_matrix4("view", m_view);
    m_spritesheet.bind(m_shaders, 0);
    m_chunk.render(m_shaders);
}