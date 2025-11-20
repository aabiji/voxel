#pragma once

#include "camera.h"
#include "terrain.h"

enum class Direction { front, back, right, left, up };

struct Selection {
    Vec3 position;
    bool selected;
    float max_select_distance;
};

class Player {
public:
    void init(Terrain* terrain);
    void move(Direction direction);
    void place_object();
    void update();

    Vec3 position() { return m_position; }
    Vec3 selected_object() { return m_selected_object.position; }
    void rotate(float x, float y) { m_camera.rotate(x, y); }
    Matrix4 view_matrix() { return m_camera.look_at(); }

private:
    void update_position();
    float apply_physics(float value, float min, float max, bool is_accel);
    void find_selected_voxel();

    Vec3 m_vel, m_accel;
    float m_friction, m_speed;
    float m_max_jump_height;
    Selection m_selected_object;

    Vec3 m_size;
    Vec3 m_position;

    Camera m_camera;
    Terrain* m_terrain;
};
