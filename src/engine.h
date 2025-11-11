#pragma once

#include "camera.h"
#include "chunk.h"
#include "spritesheet.h"

class Player
{
public:
    Player()
    {
        float middle = float(CHUNK_SIZE) / 2;
        m_position = Vec3(middle, 0, middle);
        m_velocity = Vec2(0.1, 0);
        m_acceleration = Vec2(0, 0.01);
        m_size = Vec3(1, 2, 1);
        camera.position =
            Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
    }

    void move(Terrain& terrain, float offsetx, float offsetz)
    {
        // TODO: why are we moving slower while pointing downwards??
        // TODO: player jumping
        // TODO: collisions at an angle

        // move forward or backwards or side to side
        Vec3 target = m_position;
        Vec3 f = Vec3(camera.front.x, 0, camera.front.z);
        Vec3 right = Vec3::cross(camera.front, camera.up).normalize();
        if (offsetz == 1) target += f * m_velocity.x;
        if (offsetz == -1) target -= f * m_velocity.x;
        if (offsetx == 1) target += right * m_velocity.x;
        if (offsetx == -1) target -= right * m_velocity.x;

        // can't move if the player will collide or step off chunk
        auto result = terrain.get_surface_y(target);
        if (result.is_err() || result.value() > m_position.y) return;

        m_position = target;
        camera.position = Vec3(target.x, target.y + m_size.y, target.z);
    }

    void fall(Terrain& terrain)
    {
        // decrease y until the surface is reached
        auto result = terrain.get_surface_y(m_position);
        if (!result.is_err()) {
            m_velocity.y += m_acceleration.y;
            m_position.y -= m_velocity.y;

            if (m_position.y < result.value()) {
                m_velocity.y = 0;
                m_position.y = result.value();
            }
        }

        camera.position =
            Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
    }

    Camera camera;
private:
    Vec3 m_size;
    Vec3 m_position;
    Vec2 m_velocity;
    Vec2 m_acceleration;
};

class Engine
{
public:
    Engine(int window_width, int window_height);

    void render();
    void move_player(int offsetx, int offsetz);

    void handle_resize(int width, int height);
    void handle_mouse_move(float x, float y);
    void disable_camera_movement() { m_camera_disabled = true; }
private:
    Matrix4 m_projection;
    bool m_camera_disabled;

    Player m_player;
    Spritesheet m_spritesheet;
    ShaderManager m_shaders;
    Terrain m_terrain;
};