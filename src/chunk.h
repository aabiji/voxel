#pragma once

#include <vector>

#include "math.h"

struct Voxel { Vec<3> position; };

class Chunk
{
public:
    Chunk();
    ~Chunk();
    void draw();
private:
    void create_vertex_buffer();
    void create_element_buffer();
    void create_storage_buffer();
    void generate_chunk();

    Vec<3> m_size;
    int m_num_indices;
    std::vector<Voxel> m_voxels;
    unsigned int m_vao; // vertex attribute array
    unsigned int m_vbo; // vertex buffer object
    unsigned int m_ebo; // element buffer object
    unsigned int m_ssbo; // shader storage buffer object
};
