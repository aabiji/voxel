#include <glad/glad.h>

#include "chunk.h"
#include "voxel.h"

Voxel::Voxel()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(voxel_vertices), voxel_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(voxel_indices), voxel_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(1); // texture coordinate
}

Voxel::~Voxel()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void Voxel::render()
{
    glBindVertexArray(m_vao);
    int length = sizeof(voxel_indices) / sizeof(unsigned int);
    glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0);
}

Chunk::Chunk()
{
    m_voxels.insert({ Vec3(0, 0, 0), true });
}

void Chunk::render(ShaderManager& shaders)
{
    for (auto& [position, _] : m_voxels) {
        Matrix4 model;
        model = model * Matrix4::translate(position);
        shaders.set_matrix4("model", model);
        m_voxel.render();
    }
}