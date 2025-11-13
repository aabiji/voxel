#include "player.h"

void Player::init(Terrain& terrain)
{
    m_velocity = Vec3(0.1, 0, 0);
    m_acceleration = Vec3(0, 0.01, 0);
    m_size = Vec3(1, 2, 1);
    m_on_ground = true;

    float middle = float(CHUNK_SIZE) / 2;
    auto result = terrain.get_surface_y(middle, middle);
    m_position = Vec3(middle, result.value(), middle);

    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}

void Player::move(Terrain& terrain, int offsetx, int offsety, int offsetz)
{
    Vec3 target_pos = m_position;
    Vec3 target_vel = m_velocity;
    Vec3 front = Vec3(m_camera.front.x, 0, m_camera.front.z).normalize();
    Vec3 right = Vec3::cross(m_camera.front, m_camera.up).normalize();

    if (offsetz == 1) // forward
        target_pos += front * m_velocity.x;
    if (offsetz == -1) // backward
        target_pos -= front * m_velocity.x;
    if (offsetx == 1) // right
        target_pos += right * m_velocity.x;
    if (offsetx == -1) // left
        target_pos -= right * m_velocity.x;
    if (offsety == -1 && m_on_ground) // jump
        target_vel.y -= 0.25f;

    // don't move if the player will collide or step off chunk
    if (!terrain.voxel_collision(target_pos + target_vel, m_size)) {
        m_position = target_pos;
        m_velocity = target_vel;
        m_camera.position = Vec3(target_pos.x, target_pos.y + m_size.y, target_pos.z);
    }
}

void Player::fall(Terrain& terrain)
{
    // decrease y until the surface is reached
    auto result = terrain.get_surface_y(m_position.x, m_position.z);
    if (!result.is_err()) {
        m_velocity.y += m_acceleration.y;
        m_position.y -= m_velocity.y;
        m_on_ground = false;

        if (m_position.y <= result.value()) {
            m_velocity.y = 0;
            m_on_ground = true;
            m_position.y = result.value();
        }
    }

    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}
