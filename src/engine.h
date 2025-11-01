#pragma once

#include "shader.h"

enum class Direction { left, right, down, up, forward, backward };

class Engine
{
public:
    Engine();

    void handle_resize(int width, int height);
    void handle_mouse_move(int x, int y);

    void render();
    void move_player(Direction direction);

private:
    int m_window_width;
    int m_window_height;

    ShaderManager m_shaders;
};