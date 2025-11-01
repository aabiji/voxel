#pragma once

#include "math.h"
#include "utils.h"

class Camera
{
public:
    Camera()
        : m_speed(0.05), m_sensitivity(0.5),
          m_pos(0, 0, -5), m_up(0, 1, 0), m_front(0, 0, -1),
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
        float yaw_delta = (mousex - m_prev_x) * m_sensitivity;
        float yaw_angle = yaw_delta * 0.5;
        m_prev_x = mousex;
        Quaternion yaw(cos(yaw_angle), 0.0, sin(yaw_angle), 0.0);

        // rotate around the x (up/down)
        float pitch_delta = (mousey - m_prev_y) * m_sensitivity;
        float pitch_angle = pitch_delta * 0.5;
        m_prev_y = mousey;
        Quaternion pitch(cos(pitch_angle), sin(pitch_angle), 0.0, 0.0);

        // rotate vectors
        m_rotation = (yaw * m_rotation * pitch).normalize();
        m_front = m_rotation.rotate(Vec3(0, 0, -1));
        m_up = m_rotation.rotate(Vec3(0, 1, 0));
    }

    Matrix4 look_at()
    {
        Vec3 direction = -m_front;
        Vec3 right = Vec3::cross(m_up, direction).normalize();
        Vec3 up = Vec3::cross(direction, right);
        // because of opengl, this is in column major ordering
        return Matrix4(
            right.x,  up.x,     direction.x, 0.0f,
            right.y,  up.y,     direction.y, 0.0f,
            right.z,  up.z,     direction.z, 0.0f,
            -m_pos.x, -m_pos.y, -m_pos.z,    1.0f
        );
    }

private:
    float m_speed, m_sensitivity;
    Vec3 m_pos, m_up, m_front;
    Quaternion m_rotation;
    float m_prev_x, m_prev_y;
    bool m_first_move;
};