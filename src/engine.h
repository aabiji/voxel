#pragma once

#include <memory>

#include "camera.h"
#include "chunk.h"
#include "spritesheet.h"

class Engine
{
public:
    Engine(int window_width, int window_height);
    void update();
    void render();

    void handle_resize(int width, int height);
    void handle_mouse_move(float x, float y);
    void move_player(int offsetx, int offsetz);

private:
    Matrix4 m_projection;
    Matrix4 m_view;

    Camera m_camera;
    Spritesheet m_spritesheet;
    ShaderManager m_shaders;
    std::unordered_map<Vec3, std::shared_ptr<Chunk>, Vec3Hasher> m_chunks;
};