#include "player.h"
#include "chunk.h"
#include "terrain.h"

void Player::init(Terrain* terrain)
{
    float mid = float(CHUNK_SIZE) / 2;
    m_position = Vec3(mid, terrain->surface_y(mid, mid), mid);
    m_size = Vec3(1, 3, 1);

    m_vel = Vec3(0.0, 0, 0);
    m_accel = Vec3(0, -0.15, 0);
    m_friction = 0.2;
    m_speed = 0.15;
    m_max_jump_height = 1.5;

    m_selected_voxel = Vec3(0, 0, 0);
    m_view_distance = 10;

    m_camera.position = Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
    m_terrain = terrain;
}

void Player::move(Direction direction)
{
    Vec3 front = m_camera.front;
    front.y = 0;
    // avoid potential NaN caused by dividing with what's essentially 0
    front = front.length() > 0.001 ? front.norm() : Vec3(0, 0, -1);

    Vec3 right = Vec3::cross(m_camera.front, m_camera.up);
    right.y = 0;
    // avoid potential NaN caused by dividing with what's essentially 0
    right = right.length() > 0.001 ? right.norm() : Vec3(1, 0, 0);

    float surface_y = m_terrain->surface_y(m_position.x, m_position.z);
    bool on_ground = m_position.y <= surface_y + 1.001;

    // move in the camera's direction
    if (direction == Direction::right)
        m_accel += right * m_speed;
    if (direction == Direction::left)
        m_accel -= right * m_speed;
    if (direction == Direction::front)
        m_accel += front * m_speed;
    if (direction == Direction::back)
        m_accel -= front * m_speed;
    if (direction == Direction::up && on_ground)
        m_vel.y = m_max_jump_height;
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
// update the player's position whilst resolving any possible coliisions on each axis
void Player::update_position()
{
    for (int axis = 0; axis < 3; axis++) {
        m_position[axis] += m_vel[axis];
        if (m_terrain->collision(m_position, m_size, 1)) {
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
            if (m_terrain->collision(m_position, m_size, 1))
                m_position[axis] -= m_vel[axis];

            m_vel[axis] = 0;
        }
    }
}

void Player::find_selected_voxel()
{
    // TODO: debug this!
    // get the voxel the player's currently looking at:
    // basically just cast a ray in the player's viewing
    // direction and sample it in steps
    float step = 0.5;
    float steps = m_view_distance / step;
    for (int i = 1; i <= steps; i++) {
        Vec3 p = m_position + m_camera.front * (i * step);
        if (m_terrain->voxel_exists(p.x, p.y, p.z)) {
            VoxelLocation l = m_terrain->voxel_location(p.x, p.z);
            float x = l.chunk_x * CHUNK_SIZE + l.voxel_x;
            float z = l.chunk_z * CHUNK_SIZE + l.voxel_z;
            m_selected_voxel = Vec3(x, std::floor(p.y), z);
            break;
        }
    }
}

void Player::update()
{
    m_vel += m_accel;
    update_position();

    m_accel.x = apply_physics(m_accel.x, 0.1, 0.15, true);
    m_accel.z = apply_physics(m_accel.z, 0.1, 0.15, true);

    m_vel.x = apply_physics(m_vel.x, 0.1, 0.15, false);
    m_vel.z = apply_physics(m_vel.z, 0.1, 0.15, false);

    m_camera.position = Vec3(m_position.x, m_position.y + m_size.y, m_position.z);

    find_selected_voxel();
}
