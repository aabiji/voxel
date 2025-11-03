#include <glad/glad.h>

#include "chunk.h"

Chunk::~Chunk()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void Chunk::init_buffers()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_vertices.size() * sizeof(Vertex),
        m_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_indices.size() * sizeof(unsigned int),
        m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(1); // texture coordinate

    m_num_indices = m_indices.size();
    m_indices.clear();
    m_vertices.clear();
}

void Chunk::compute_mesh()
{
    m_indices.clear();
    m_vertices.clear();
    auto voxel_faces = get_voxel_faces();
    int quad_indices[] = { 0, 1, 2, 0, 2, 3 };

    for (const auto& [p, _] : m_voxels) {
        for (const auto& [face, vertices] : voxel_faces) {
            // computing mesh top down
            Vec3 face_position = Vec3(p.x + face.x, p.y - face.y, p.z + face.z);
            // only add vertices for voxel faces that aren't occluded
            if (!m_voxels.count(face_position)) {
                unsigned int base_index = m_vertices.size();

                for (const Vertex v : vertices) {
                    Vertex modified = v;
                    // apply translation
                    modified.x += m_position.x + p.x;
                    modified.y += m_position.y - p.y;
                    modified.z += m_position.z + p.z;
                    // only use grass-side sprite for top layer voxels
                    if (p.y != 0 && modified.w == 0)
                        modified.w = 2;
                    m_vertices.push_back(modified);
                }

                // indices for the quad
                for (int i = 0; i < 6; i++) {
                    m_indices.push_back(base_index + quad_indices[i]);
                }
            }
        }
    }
}

void Chunk::generate()
{
    // TODO: voxel generation based off of a noise functino goes here:
    m_position = Vec3(-5, -2, -3);
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 4; y++) {
            for (int z = 0; z < 6; z++) {
                m_voxels.insert({ Vec3(x, y, z), true });
            }
        }
    }
    compute_mesh();
    init_buffers();
}

void Chunk::render()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
}