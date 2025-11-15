#pragma once

#include "camera.h"
#include "terrain.h"

class Player
{
public:
    void init(Terrain& terrain);
    void fall(Terrain& terrain);
    void move(Terrain& terrain, int offsetx, int offsety, int offsetz);
    void resolve_collisions(Terrain& terrain);

    Vec3 get_position() { return m_position; }
    void rotate(float x, float y) { m_camera.rotate(x, y); }
    Matrix4 view_matrix() { return m_camera.look_at(); }
private:
    bool m_on_ground;
    Vec3 m_position, m_size;
    Vec3 m_velocity, m_acceleration;
    Camera m_camera;
};
