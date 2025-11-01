#pragma once

#include <unordered_map>

#include "math.h"

class Voxel
{
public:
    Voxel();
    ~Voxel();
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
    void render();
private:
    // Map a voxel to its position
    std::unordered_map<Vec3, Voxel, Vec3Hasher> m_voxels;
};