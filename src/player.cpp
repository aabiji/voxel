#include "player.h"
#include "chunk.h"

void Player::init(Terrain& terrain)
{
    float mid = float(CHUNK_SIZE) / 2;
    m_velocity = Vec3(0.1, 0, 0);
    m_acceleration = Vec3(0, -0.01, 0);
    m_size = Vec3(1, 2, 1);
    m_on_ground = true;
    m_position = Vec3(mid, terrain.surface_y(mid, mid), mid);
    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}

bool Player::collision(Terrain& terrain, Vec3 target_pos)
{
    for (int x = ceil(target_pos.x); x < ceil(target_pos.x + m_size.x); x++) {
        for (int y = ceil(target_pos.y + m_size.y + 1); y >= ceil(target_pos.y + 1); y--) {
            for (int z = ceil(target_pos.z); z < ceil(target_pos.z + m_size.z); z++) {
                log("({}, {}, {})", x, y, z);
                if (terrain.voxel_exists(x, y, z)) return true;
            }
        }
    }
    return false;
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
        target_vel.y += 0.25f;

    // don't move if the player will collide or step off chunk
    if (!collision(terrain, target_pos)) {
        m_position = target_pos;
        m_velocity = target_vel;
        m_camera.position = Vec3(target_pos.x, target_pos.y + m_size.y, target_pos.z);
    }
}

void Player::fall(Terrain& terrain)
{
    // decrease y until the surface is reached
    float surface_y = terrain.surface_y(m_position.x, m_position.z);

    m_velocity.y += m_acceleration.y;
    m_position.y += m_velocity.y;
    m_on_ground = false;

    if (m_position.y <= surface_y) {
        m_velocity.y = 0;
        m_on_ground = true;
        m_position.y = surface_y;
    }

    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}
