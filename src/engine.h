#pragma once

#include "player.h"
#include "spritesheet.h"

class Engine {
public:
    Engine(float window_width, float window_height);

    void render();
    void move_player(Direction direction);

    void handle_resize(int width, int height);
    void handle_mouse_move(float x, float y);
    void disable_camera_movement() { m_camera_disabled = true; }

private:
    void load_assets();

    bool m_camera_disabled;
    Vec2 m_window_size;

    Player m_player;
    Terrain m_terrain;
    Spritesheet m_spritesheet;

    Matrix4 m_projection;
    ShaderManager m_shaders;
};
