#pragma once

#include <vector>

#include "vertex.h"

const int CHUNK_SIZE = 20;
const int CHUNK_HEIGHT = 20;

class Chunk
{
public:
    Chunk(Vec3 position);
    ~Chunk();

    // disable copy and move constructors
    Chunk& operator=(const Chunk&) = delete;
    Chunk(const Chunk&) = delete;
    Chunk& operator=(Chunk&) = delete;
    Chunk(Chunk&) = delete;

    void render();
    bool voxel_present(Vec3 position);
    float get_surface_y(float x, float z);
private:
    void compute_mesh();
    void init_buffers();

    int m_num_indices;
    unsigned int m_vao, m_vbo, m_ebo;
    // cleared after the chunk is generated
    std::vector<unsigned int> m_indices;
    std::vector<Vertex> m_vertices;

    Vec3 m_position;
    std::unordered_map<Vec3, bool, Vec3Hasher> m_voxels;
};
