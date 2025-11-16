#pragma once

#include "camera.h"
#include "terrain.h"

enum class Direction { front, back, right, left, up };

class Player {
public:
    void init(Terrain* terrain);
    void move(Direction direction);
    void update();

    Vec3 get_position() { return m_position; }
    void rotate(float x, float y) { m_camera.rotate(x, y); }
    Matrix4 view_matrix() { return m_camera.look_at(); }

private:
    bool check_collision();
    void update_position();
    float apply_physics(float value, float min, float max, bool is_accel);

    Vec3 m_vel, m_accel;
    float m_friction, m_speed;
    float m_max_jump_height;

    Vec3 m_size;
    Vec3 m_position;

    Camera m_camera;
    Terrain* m_terrain;
};
