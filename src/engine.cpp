#include <glad/glad.h>

#include "engine.h"

Engine::Engine(int window_width, int window_height)
{
    auto result = m_shaders.load(
        "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    if (result.is_err())
        log(Level::fatal, result.error());

    m_spritesheet.load("assets/textures/atlas.png", 64, 3);

    glViewport(0, 0, window_width, window_height);
    float aspect = float(window_width) / float(window_height);
    m_projection = Matrix4::projection(0.1f, 100.0f, 45 * (M_PI / 180.0f), aspect);
    m_view = m_camera.look_at();

    // create the initial chunk
    auto chunk = std::make_shared<Chunk>(Vec3(0, 0, 0));
    m_chunks.insert({ Vec3(0, 0, 0), chunk });
}

void Engine::handle_mouse_move(float x, float y)
{
    m_camera.rotate(x, y);
    m_view = m_camera.look_at();
}

void Engine::move_player(int offsetx, int offsetz)
{
    m_camera.move(offsetx, offsetz);
    m_view = m_camera.look_at();
}

void Engine::handle_resize(int width, int height)
{
    glViewport(0, 0, width, height);
    float aspect = float(width) / float(height);
    m_projection = Matrix4::projection(0.1, 100.0, 45 * (M_PI / 180), aspect);
}

void Engine::update()
{
    // make the player fall to the surface of the voxel it's currently on
    Vec3 p = m_camera.get_pos();
    float chunk_x = floor(p.x / float(CHUNK_SIZE));
    float chunk_z = floor(p.z / float(CHUNK_SIZE));
    float player_x = floor(p.x - chunk_x * CHUNK_SIZE);
    float player_z = floor(p.z - chunk_z * CHUNK_SIZE);

    auto chunk = m_chunks.find(Vec3(chunk_x, 0, chunk_z));
    if (chunk != m_chunks.end()) {
        float y = chunk->second->get_surface_y(player_x, player_z);
        m_camera.fall(y + 2); // TODO: get an actual player height
    }
}

void Engine::render()
{
    m_shaders.use();
    m_shaders.set_matrix4("projection", m_projection);
    m_shaders.set_matrix4("view", m_view);
    m_spritesheet.bind(m_shaders, 0);

    for (auto& [_, chunk] : m_chunks)
        chunk->render();
}