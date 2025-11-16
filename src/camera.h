#pragma once

#include "math.h"
#include "utils.h"

class Camera
{
public:
    Camera()
    {
        position = Vec3(0, 0, 0);
        up = Vec3(0, 1, 0);
        front = Vec3(0, 0, -1),
        m_sensitivity = 0.003;
        m_yaw = m_pitch = 0;
        m_first_move = true;
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
        m_rotation = (yaw * pitch).norm();
        front = m_rotation.rotate(Vec3(0, 0, -1));
        up = m_rotation.rotate(Vec3(0, 1, 0));
    }

    Matrix4 look_at()
    {
        Vec3 f = front.norm();
        Vec3 right = Vec3::cross(f, up).norm();
        Vec3 up = Vec3::cross(right, f);

        Matrix4 m;
        m.values[0]  = right.x;
        m.values[1]  = up.x;
        m.values[2]  = -f.x;
        m.values[3]  = 0.0f;

        m.values[4]  = right.y;
        m.values[5]  = up.y;
        m.values[6]  = -f.y;
        m.values[7]  = 0.0f;

        m.values[8]  = right.z;
        m.values[9]  = up.z;
        m.values[10] = -f.z;
        m.values[11] = 0.0f;

        m.values[12] = -Vec3::dot(right, position);
        m.values[13] = -Vec3::dot(up, position);
        m.values[14] =  Vec3::dot(f, position);
        m.values[15] = 1.0f;
        return m;
    }

    Vec3 position, up, front;

private:
    float m_sensitivity;
    float m_yaw, m_pitch;
    float m_prev_x, m_prev_y;
    Quaternion m_rotation;
    bool m_first_move;
};
