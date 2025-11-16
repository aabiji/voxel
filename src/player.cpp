#include "player.h"
#include "chunk.h"

void Player::init(Terrain& terrain)
{
    float mid = float(CHUNK_SIZE) / 2;
    m_position = Vec3(mid, terrain.surface_y(mid, mid), mid);
    m_size = Vec3(1, 2, 1);

    m_vel = Vec3(0.0, 0, 0);
    m_accel = Vec3(0, 0, 0);
    m_friction = 0.2;
    m_speed = 0.5;

    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}

void Player::move(Terrain& terrain, Direction direction)
{
    Vec3 front = m_camera.front.norm();
    Vec3 right = Vec3::cross(m_camera.front, m_camera.up).norm();
    front.y = right.y = 0; // only move laterally or front to back

    // move in the camera's direction
    if (direction == Direction::right) m_accel += right * m_speed;
    if (direction == Direction::left) m_accel -= right * m_speed;
    if (direction == Direction::front) m_accel += front * m_speed;
    if (direction == Direction::back) m_accel -= front * m_speed;
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

struct BoundingBox
{
    BoundingBox(Matrix4 m)
    {
        Vec3 scaled1 = Vec3(m.values[0], m.values[1], m.values[2]);  // right (column 0)
        Vec3 scaled2 = Vec3(m.values[4], m.values[5], m.values[6]);  // up (column 1)
        Vec3 scaled3 = Vec3(m.values[8], m.values[9], m.values[10]); // forward (column 2)

        half_extents = Vec3(scaled1.length(), scaled2.length(), scaled3.length());
        center = Vec3(m.values[12], m.values[13], m.values[14]).norm(); // forward (column 2)
        axis1 = scaled1.norm();
        axis2 = scaled2.norm();
        axis3 = scaled3.norm();
    }
private:
    Vec3 center, half_extents;
    Vec3 axis1, axis2, axis3;
};

bool collision(Matrix4 a, Matrix4 b)
{
    BoundingBox box_a = BoundingBox(a);
    BoundingBox box_b = BoundingBox(b);
    // TODO: implement the algorithm!
    return false;
}

void Player::update(Terrain& terrain)
{
    m_vel += m_accel;
    m_position += m_vel;

    m_accel.x = apply_physics(m_accel.x, 0.1, 0.25, true);
    m_accel.y = apply_physics(m_accel.y, 0.1, 0.25, true);
    m_accel.z = apply_physics(m_accel.z, 0.1, 0.25, true);

    m_vel.x = apply_physics(m_vel.x, 0.1, 0.3, false);
    m_vel.y = apply_physics(m_vel.y, 0.1, 0.3, false);
    m_vel.z = apply_physics(m_vel.z, 0.1, 0.3, false);

    Matrix4 player;
    player *= Matrix4::scale(m_size);
    player *= Quaternion(Vec3(1, 0, 0), 0).rotation();
    player *= Matrix4::translate(m_position);

    Matrix4 voxel;
    voxel *= Matrix4::translate(m_position);

    collision(player, voxel);

    //log("acceleration ({}, {}, {})", m_accel.x, m_accel.y, m_accel.z);
    //log("velocity ({}, {}, {})", m_vel.x, m_vel.y, m_vel.z);
    //log("position ({}, {}, {})\n", m_position.x, m_position.y, m_position.z);

    m_camera.position =
        Vec3(m_position.x, m_position.y + m_size.y, m_position.z);
}
