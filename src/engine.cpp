#include <glad/glad.h>

#include "engine.h"

Engine::Engine(int window_width, int window_height)
    : m_chunk(Vec3(0, 0, 0))
{
    auto result = m_shaders.load(
        "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    m_spritesheet.load("assets/textures/atlas.png", 64, 3);

    glViewport(0, 0, window_width, window_height);
    float aspect = float(window_width) / float(window_height);
    log(Level::info, "Aspect: {}, Screen: {}, {}", aspect, window_width, window_height);
    m_projection = Matrix4::projection(0.1f, 100.0f, 45 * (M_PI / 180.0f), aspect);
    m_view = m_camera.look_at();

    m_chunk.generate();
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