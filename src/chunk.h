#pragma once

#include <unordered_map>

#include "shader.h"

class Voxel
{
public:
    Voxel();
    ~Voxel();

    Voxel(const Voxel&) = delete;
    Voxel(Voxel&) = delete;
    Voxel& operator=(const Voxel&) = delete;
    Voxel& operator=(Voxel&) = delete;

    void render();
private:
    unsigned int m_vao; // vertex attribute array
    unsigned int m_vbo; // vertex buffer object
    unsigned int m_ebo; // element buffer object
};

class Chunk
{
public:
    Chunk();
    void render(ShaderManager& shaders);
private:
    // Map a voxel to its position
    Voxel m_voxel;
    std::unordered_map<Vec3, bool, Vec3Hasher> m_voxels;
};