#pragma once

#include "math.h"

enum class Direction { FRONT, BACK, LEFT, RIGHT, UP, DOWN };

class Camera
{
public:
    Camera() : m_position(0.0, 0.0, 5.0), m_front(0, 0, -1), m_up(0, 1, 0)
    {
        m_yaw = 0, m_pitch = 0;
        m_previous_x = 0, m_previous_y = 0;
    }

    void set_initial_mouse_position(float x, float y)
    {
        m_previous_x = x;
        m_previous_y = y;
    }

    Matrix<4, 4> view_matrix()
    {
        auto target = m_position + m_front;
        return look_at(m_position, target, m_up);
    }

    void rotate(float mouseX, float mouseY)
    {
        float sensitivity = 0.002;
        float deltaX = mouseX - m_previous_x;
        float deltaY = m_previous_y - mouseY;

        m_yaw = std::max(-180.0f, std::min(m_yaw + deltaX * sensitivity, 180.0f));
        m_pitch = std::max(-89.0f, std::min(m_pitch + deltaY * sensitivity, 89.0f));

        auto rotation = Quaternion(m_yaw, { 0, 1, 0 }) * Quaternion(m_pitch, { 1, 0, 0 });
        m_front = rotation * Vec<3>(0, 0, -1);
        m_up = rotation * Vec<3>(0, 1, 0);

        m_previous_x = mouseX;
        m_previous_y = mouseY;
    }

    void move(float delta_time, Direction d)
    {
        float speed = 10 * delta_time;
        auto right = Vec<3>::cross(m_front, m_up).normalize() * speed;
        if (d == Direction::FRONT) m_position += m_front * speed;
        if (d == Direction::BACK) m_position -= m_front * speed;
        if (d == Direction::RIGHT) m_position -= right;
        if (d == Direction::LEFT) m_position += right;
        if (d == Direction::UP) m_position += m_up * speed;
        if (d == Direction::DOWN) m_position -= m_up * speed;
    }

private:
    Vec<3> m_position, m_front, m_up;
    float m_previous_x, m_previous_y;
    float m_yaw, m_pitch;
};
