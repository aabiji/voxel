#pragma once

#include "chunk.h"
#include "player.h"
#include "spritesheet.h"

class Engine
{
public:
    Engine(int window_width, int window_height);

    void render();
    void move_player(int x, int y, int z);
    void jump_player();

    void handle_resize(int width, int height);
    void handle_mouse_move(float x, float y);
    void disable_camera_movement() { m_camera_disabled = true; }
private:
    Matrix4 m_projection;
    bool m_camera_disabled;

    Player m_player;
    Spritesheet m_spritesheet;
    ShaderManager m_shaders;
    Terrain m_terrain;
};
