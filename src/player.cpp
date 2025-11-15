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

struct AABB
{
    bool intersect(AABB b)
    {
        bool intersect_x = min_x < b.max_x && max_x > b.min_x;
        bool intersect_y = min_y < b.max_y && max_y > b.min_y;
        bool intersect_z = min_z < b.max_z && max_z > b.min_z;
        return intersect_x && intersect_y && intersect_z;
    }

    // return true if there's a collision and offset
    // the current bounding box to avoid the collision
    bool resolve_collision(AABB b)
    {
        if (!intersect(b)) return false;

        float overlap_x = std::min(max_x, b.max_x) - std::max(min_x, b.min_x);
        float overlap_y = std::min(max_y, b.max_y) - std::max(min_y, b.min_y);
        float overlap_z = std::min(max_z, b.max_z) - std::max(min_z, b.min_z);

        // find the axis with the minimum overlap
        if (overlap_x < overlap_y && overlap_x < overlap_z) {
            // resolve collision
            if (min_x < b.min_x) {
                // on the left of b
                min_x -= overlap_x;
                max_x -= overlap_x;
            } else {
                // on the right of b
                min_x += overlap_x;
                max_x += overlap_x;
            }
        } else if (overlap_y < overlap_x && overlap_y < overlap_z) {
            if (min_y < b.min_y) {
                min_y -= overlap_y;
                max_y -= overlap_y;
            } else {
                min_y += overlap_y;
                max_y += overlap_y;
            }
        } else {
            if (min_z < b.min_z) {
                min_z -= overlap_z;
                max_z -= overlap_z;
            } else {
                min_z += overlap_z;
                max_z += overlap_z;
            }
        }

        return true;
    }

    float min_x, min_y, min_z;
    float max_x, max_y, max_z;
};

void Player::resolve_collisions(Terrain& terrain)
{
    AABB player = {
        .min_x = m_position.x,
        .min_y = m_position.y + 1,
        .min_z = m_position.z,
        .max_x = m_position.x + m_size.x,
        .max_y = m_position.y + m_size.y + 1,
        .max_z = m_position.z + m_size.z,
    };

    for (int x = player.min_x; x <= player.max_x; x++) {
        for (int y = player.min_y; y <= player.max_y; y++) {
            for (int z = player.min_z; z <= player.max_z; z++) {
                AABB voxel = {
                    .min_x = float(x), .min_y = float(y),
                    .min_z = float(z), .max_x = float(x + 1),
                    .max_y = float(y + 1), .max_z = float(z + 1)
                };

                bool voxel_exists = terrain.voxel_exists(voxel.min_x, voxel.min_y, voxel.min_z);
                if (voxel_exists && player.resolve_collision(voxel)) {
                    //log("COLLISION!: ({}, {}, {}) with ({}, {}, {})", player.min_x, player.min_y, player.min_z, x, y, z);
                    //log("NEW POS!: ({}, {}, {})", player.min_x, player.min_y, player.min_z);
                }
            }
        }
    }

    m_position.x = player.min_x;
    //m_position.y = player.min_y;
    m_position.z = player.min_z;
}

void Player::move(Terrain& terrain, int offsetx, int offsety, int offsetz)
{
    Vec3 front = Vec3(m_camera.front.x, 0, m_camera.front.z).normalize();
    Vec3 right = Vec3::cross(m_camera.front, m_camera.up).normalize();

    if (offsetz == 1) // forward
        m_position += front * m_velocity.x;
    if (offsetz == -1) // backward
        m_position -= front * m_velocity.x;
    if (offsetx == 1) // right
        m_position += right * m_velocity.x;
    if (offsetx == -1) // left
        m_position -= right * m_velocity.x;
    if (offsety == -1 && m_on_ground) // jump
        m_velocity.y += 0.25f;
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

    m_camera.position = Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}
