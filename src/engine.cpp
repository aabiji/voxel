#include <glad/glad.h>

#include "engine.h"

Engine::Engine(float window_width, float window_height)
{
    auto result
        = m_shaders.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    result = m_spritesheet.load("assets/textures/atlas.png", 64, 3);
    if (result.is_err())
        log(Level::fatal, result.error());

    glViewport(0, 0, window_width, window_height);
    m_projection = Matrix4::projection(
        0.1f, 100.0f, 45 * (M_PI / 180.0f), window_width / window_height);
    m_window_size = Vec2(window_width, window_height);
    m_camera_disabled = false;

    m_terrain.load_more_chunks(0, 0);
    m_player.init(&m_terrain);
}

void Engine::move_player(Direction direction) { m_player.move(direction); }

void Engine::handle_mouse_move(float x, float y)
{
    if (!m_camera_disabled)
        m_player.rotate(x, y);
}

void Engine::handle_resize(int width, int height)
{
    glViewport(0, 0, width, height);
    float aspect = float(width) / float(height);
    m_projection = Matrix4::projection(0.1, 100.0, 45 * (M_PI / 180), aspect);
    m_window_size = Vec2(width, height);
}

void Engine::render()
{
    Vec3 p = m_player.get_position();
    m_terrain.load_more_chunks(p.x, p.z);
    m_player.update();

    m_shaders.use();
    m_shaders.set_matrix4("projection", m_projection);
    m_shaders.set_matrix4("view", m_player.view_matrix());
    m_shaders.set_vec3("selected_world_pos", Vec3(floor(p.x), floor(p.y), floor(p.z)));

    m_spritesheet.bind(m_shaders, 0);
    m_terrain.render();
}
