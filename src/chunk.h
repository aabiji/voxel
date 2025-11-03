#pragma once

#include <vector>

#include "vertex.h"

class Chunk
{
public:
    Chunk(Vec3 position) : m_position(position) {}
    ~Chunk();

    // disable copy and move constructors
    Chunk& operator=(const Chunk&) = delete;
    Chunk(const Chunk&) = delete;
    Chunk& operator=(Chunk&) = delete;
    Chunk(Chunk&) = delete;

    void generate();
    void render();
private:
    void compute_mesh();
    void init_buffers();

    int m_num_indices;
    std::vector<unsigned int> m_indices;
    std::vector<Vertex> m_vertices;
    unsigned int m_vao, m_vbo, m_ebo;

    Vec3 m_position;
    std::unordered_map<Vec3, bool, Vec3Hasher> m_voxels;
};