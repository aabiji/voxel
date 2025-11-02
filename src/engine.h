#pragma once

#include "camera.h"
#include "chunk.h"
#include "spritesheet.h"

class Engine
{
public:
    Engine();
    void render();

    void handle_resize(int width, int height);
    void handle_mouse_move(float x, float y);
    void move_player(Direction direction);

private:
    Matrix4 m_projection;
    Matrix4 m_view;
    int m_window_width;
    int m_window_height;

    Chunk m_chunk;
    Camera m_camera;
    Spritesheet m_spritesheet;
    ShaderManager m_shaders;
};