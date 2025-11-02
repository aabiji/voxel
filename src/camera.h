#pragma once

#include "math.h"
#include "utils.h"

class Camera
{
public:
    Camera()
        : m_speed(0.05), m_sensitivity(0.003),
          m_yaw(0), m_pitch(0),
          m_pos(0, 0, 6), m_up(0, 1, 0), m_front(0, 0, -1),
          m_first_move(true) {}

    void move(Direction direction)
    {
        Vec3 up = m_up * m_speed;
        Vec3 front = m_front * m_speed;
        Vec3 right = Vec3::cross(m_front, m_up).normalize() * m_speed;

        if (direction == Direction::up) m_pos += up;
        if (direction == Direction::down) m_pos -= up;
        if (direction == Direction::front) m_pos += front;
        if (direction == Direction::back) m_pos -= front;
        if (direction == Direction::right) m_pos += right;
        if (direction == Direction::left) m_pos -= right;
    }

    void rotate(float mousex, float mousey)
    {
        // avoid a huge camera jump at the first mouse move
        if (m_first_move) {
            m_first_move = false;
            m_prev_x = mousex;
            m_prev_y = mousey;
            return;
        }

        // rotate around the y (left/right)
        float yaw_delta = (m_prev_x - mousex) * m_sensitivity;
        m_yaw += yaw_delta;
        m_prev_x = mousex;
        Quaternion yaw(cos(m_yaw * 0.5), 0.0, sin(m_yaw * 0.5), 0.0);

        // rotate around the x (up/down), avoiding gimbal flip
        float pitch_delta = (m_prev_y - mousey) * m_sensitivity;
        float max_pitch = M_PI / 2.0;
        m_pitch = std::max(-max_pitch, std::min(max_pitch, m_pitch + pitch_delta));
        m_prev_y = mousey;
        Quaternion pitch(cos(m_pitch * 0.5),  sin(m_pitch * 0.5), 0.0, 0.0);

        // rotate vectors
        m_rotation = (yaw * pitch).normalize();
        m_front = m_rotation.rotate(Vec3(0, 0, -1));
        m_up = m_rotation.rotate(Vec3(0, 1, 0));
    }

    Matrix4 look_at()
    {
        Vec3 front = m_front.normalize();
        Vec3 right = Vec3::cross(front, m_up).normalize();
        Vec3 up = Vec3::cross(right, front);

        Matrix4 m;
        m.values[0]  = right.x;
        m.values[1]  = up.x;
        m.values[2]  = -front.x;
        m.values[3]  = 0.0f;

        m.values[4]  = right.y;
        m.values[5]  = up.y;
        m.values[6]  = -front.y;
        m.values[7]  = 0.0f;

        m.values[8]  = right.z;
        m.values[9]  = up.z;
        m.values[10] = -front.z;
        m.values[11] = 0.0f;

        m.values[12] = -Vec3::dot(right, m_pos);
        m.values[13] = -Vec3::dot(up, m_pos);
        m.values[14] =  Vec3::dot(front, m_pos);
        m.values[15] = 1.0f;
        return m;
    }

private:
    float m_speed;
    float m_sensitivity;
    float m_yaw, m_pitch;
    Vec3 m_pos, m_up, m_front;
    float m_prev_x, m_prev_y;
    Quaternion m_rotation;
    bool m_first_move;
};