#include "player.h"
#include "chunk.h"
#include "terrain.h"

void Player::init(Terrain* terrain)
{
    float mid = float(CHUNK_SIZE) / 2;
    m_position = Vec3(mid, terrain->surface_y(mid, mid), mid);
    m_size = Vec3(1, 2, 1);

    m_vel = Vec3(0.0, 0, 0);
    m_accel = Vec3(0, -0.25, 0);
    m_friction = 0.2;
    m_speed = 0.2;

    m_camera.position = Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
    m_terrain = terrain;
}

void Player::move(Direction direction)
{
    Vec3 front = m_camera.front.norm();
    Vec3 right = Vec3::cross(m_camera.front, m_camera.up).norm();
    front.y = right.y = 0; // only move laterally or front to back

    // move in the camera's direction
    if (direction == Direction::right)
        m_accel += right * m_speed;
    if (direction == Direction::left)
        m_accel -= right * m_speed;
    if (direction == Direction::front)
        m_accel += front * m_speed;
    if (direction == Direction::back)
        m_accel -= front * m_speed;
}

// The value is acceleration or velocity
// Clamp to the min or the max and either;
// - apply friction in the opposite direction of the value (accelration)
// - decrease the magnitude of the value (velocity)
float Player::apply_physics(float value, float min, float max, bool is_accel)
{
    float direction = value < 0 ? -1 : 1;

    if (value > -min && value < min)
        value = 0;
    else if (value < -max || value > max)
        value = direction * max;
    else {
        if (is_accel)
            value += -direction * m_friction;
        else
            value *= (1.0 - m_friction);
    }

    return value;
}

// check if the player's bounding box is overlapping any voxels
bool Player::check_collision()
{
    const float epsilon = 0.001; // prevents edge alignment bugs

    int min_x = std::floor(m_position.x);
    int max_x = std::floor(m_position.x + m_size.x - epsilon);

    // using y + 1 to ignore collisions with the ground (those are handled
    // separately)
    int min_y = std::floor(m_position.y + 1);
    int max_y = std::floor(m_position.y + m_size.y + 1 - epsilon);

    int min_z = std::floor(m_position.z);
    int max_z = std::floor(m_position.z + m_size.z - epsilon);

    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            for (int z = min_z; z <= max_z; z++) {
                if (m_terrain->voxel_exists(x, y, z))
                    return true;
            }
        }
    }

    return false;
}

// update the player's position whilst resolving any possible coliisions on each axis
void Player::update_position()
{
    for (int axis = 0; axis < 3; axis++) {
        m_position[axis] += m_vel[axis];
        if (check_collision()) {
            // colliding on the right? push left
            // colliding on top? push down
            // colliding at the front? push back
            if (m_vel[axis] > 0)
                m_position[axis]
                    = std::floor(m_position[axis] + m_size[axis]) - m_size[axis];

            // colliding on the left? push right
            // colliding on down? push up
            // colliding at the back? push front
            else
                m_position[axis] = std::ceil(m_position[axis]);

            // undo movement entirely if we're still colliding
            if (check_collision())
                m_position[axis] -= m_vel[axis];

            m_vel[axis] = 0;
        }
    }
}

void Player::update()
{
    m_vel += m_accel;
    update_position();

    m_accel.x = apply_physics(m_accel.x, 0.1, 0.25, true);
    m_accel.z = apply_physics(m_accel.z, 0.1, 0.25, true);

    m_vel.x = apply_physics(m_vel.x, 0.1, 0.3, false);
    m_vel.z = apply_physics(m_vel.z, 0.1, 0.3, false);

    m_camera.position = Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}
