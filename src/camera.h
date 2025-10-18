#pragma once

#include "math.h"

enum class Direction { FORWARD, BACK, LEFT, RIGHT, UP, DOWN };

class Camera
{
public:
    Camera() : m_position(0.0, 0.0, 5.0)
    {
        m_yaw = 0, m_pitch = 0;
        m_previous_x = 0, m_previous_y = 0;
        m_first_move = true;
    }

    Matrix<4, 4> view_matrix()
    {
        return LookAt(m_position, m_position + front(), up());
    }

    void rotate(float mouseX, float mouseY)
    {
        float sensitivity = 0.002;
        float deltaX = mouseX - m_previous_x;
        float deltaY = m_previous_y - mouseY;
        if (!m_first_move) {
            m_yaw = std::max(-180.0f, std::min(m_yaw + deltaX * sensitivity, 180.0f));
            m_pitch = std::max(-89.0f, std::min(m_pitch + deltaY * sensitivity, 89.0f));
        }

        m_previous_x = mouseX;
        m_previous_y = mouseY;
        m_first_move = false;
    }

    void move(float delta_time, Direction d)
    {
        float speed = 10 * delta_time;
        auto right = Vec<3>::cross(front(), up()).normalize() * speed;
        if (d == Direction::FORWARD) m_position += front() * speed;
        if (d == Direction::BACK) m_position -= front() * speed;
        if (d == Direction::RIGHT) m_position -= right;
        if (d == Direction::LEFT) m_position += right;
        if (d == Direction::UP) m_position += up() * speed;
        if (d == Direction::DOWN) m_position -= up() * speed;
    }

private:
    Vec<3> front()
    {
        auto rotation = Quaternion(m_yaw, { 0, 1, 0 }) * Quaternion(m_pitch, { 1, 0, 0 });
        return rotation * Vec<3>(0, 0, -1);
    }

    Vec<3> up()
    {
        auto rotation = Quaternion(m_yaw, { 0, 1, 0 }) * Quaternion(m_pitch, { 1, 0, 0 });
        return rotation * Vec<3>(0, 1, 0);
    }

    Vec<3> m_position;
    float m_yaw, m_pitch;
    float m_previous_x;
    float m_previous_y;
    bool m_first_move;
};
