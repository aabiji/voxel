#include "chunk.h"

const float cube_vertices[] = {
    // front face vertices
   -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,

    // back face vertices
   -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
   -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,

    // top face vertices
   -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
   -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,

    // bottom face vertices
   -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 2.0f,
    1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 2.0f,
    1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 2.0f,
   -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 2.0f,

    // right face vertices
    1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
    1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
    1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

    // left face vertices
    -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f
};

Chunk::Chunk()
{
    int width = 5, height = 5, depth = 5;
    m_voxels.reserve(width * height * depth);
    m_size = Vec<3>(width, height, depth);

    generate_chunk();
    create_vertex_buffer();
    create_storage_buffer();
    create_element_buffer();
}

Chunk::~Chunk()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Chunk::draw([[maybe_unused]]Shader& shader)
{
    // draw all the voxels with a single draw call
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
    glDrawElementsInstanced(
        GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0, m_voxels.size());
}

void Chunk::create_vertex_buffer()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    int stride = 6 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // texture coordinate
}

void Chunk::create_storage_buffer()
{
    // the uniform buffer object will store voxel data
    // note that the buffer binding = 0
    glGenBuffers(1, &m_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        m_voxels.size() * sizeof(Voxel),
        m_voxels.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
}

void Chunk::create_element_buffer()
{
    // cube face indices
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    // construct the element buffer
    std::vector<unsigned int> buffer;
    for (int z = 0; z < m_size.z(); z++) {
        for (int y = 0; y < m_size.y(); y++) {
            for (int x = 0; x < m_size.x(); x++) {
                // indexes of the triangles composing the shown cube faces
                std::vector<unsigned int> indexes;

                // only show faces that aren't occluded by neighboring cubes
                if (x == 0) // left
                    indexes.insert(indexes.end(), { 20, 21, 22, 20, 22, 23 });

                if (x == m_size.x() - 1) // right
                    indexes.insert(indexes.end(), { 16, 17, 18, 16, 18, 19 });

                if (y == 0) // show bottom
                    indexes.insert(indexes.end(), { 12, 14, 13, 12, 15, 14 });

                if (y == m_size.y() - 1) // top
                    indexes.insert(indexes.end(), { 8, 9, 10, 8, 10, 11 });

                if (z == 0) // back
                    indexes.insert(indexes.end(), { 4, 5, 6, 4, 6, 7 });

                if (z == m_size.z() - 1) // front
                    indexes.insert(indexes.end(), { 0, 1, 2, 0, 2, 3 });

                buffer.insert(buffer.end(), indexes.begin(), indexes.end());
            }
        }
    }

    // send to gpu
    m_num_indices = buffer.size();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        buffer.size() * sizeof(unsigned int),
        buffer.data(), GL_STATIC_DRAW);
}

void Chunk::generate_chunk()
{
    float block_size = 0.5;
    Vec<3> start(-2.0, 0, -2.0);

    for (int z = 0; z < m_size.z(); z++) {
        for (int y = 0; y < m_size.y(); y++) {
            for (int x = 0; x < m_size.x(); x++) {
                Vec<3> position(start.x() + x * block_size, start.y() + y * block_size, start.z() + z * block_size);
                m_voxels.push_back({ .position = position });
            }
        }
    }
}
