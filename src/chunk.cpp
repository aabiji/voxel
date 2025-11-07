#include <assert.h>
#include <glad/glad.h>
#include <random>

#include "chunk.h"
#include "utils.h"

// create a random gradient vector for a position.
// using a hash so the gradient vectors are reproducible
inline Vec2 get_gradient(int x, int y)
{
    uint32_t seed = x * 374761393u + y * 668265263u; // Large primes
    seed = (seed ^ (seed >> 13)) * 1274126177u;
    float angle = (seed & 0xFFFF) / 65535.0f * 2.0f * M_PI;
    return Vec2(angle);
}

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float perlin_noise(Vec2 point)
{
    int x = floor(point.x);
    int y = floor(point.y);

    // get gradient vectors for each corner
    Vec2 tl = get_gradient(x, y);
    Vec2 tr = get_gradient(x + 1, y);
    Vec2 bl = get_gradient(x, y + 1);
    Vec2 br = get_gradient(x + 1, y + 1);

    // get the offset vectors (distance from the corners to the point)
    Vec2 offset_tl = Vec2(point.x - x, point.y - y);
    Vec2 offset_bl = Vec2(point.x - x, point.y - (y + 1));
    Vec2 offset_tr = Vec2(point.x - (x + 1), point.y - y);
    Vec2 offset_br = Vec2(point.x - (x + 1), point.y - (y + 1));

    // smooth the fractional position
    float u = fade(point.x - x);
    float v = fade(point.y - y);

    // interpolate the position between the the scalar influence values from each corner
    float a = std::lerp(Vec2::dot(tl, offset_tl), Vec2::dot(tr, offset_tr), u);
    float b = std::lerp(Vec2::dot(bl, offset_bl), Vec2::dot(br, offset_br), u);
    float noise = std::lerp(a, b, v);

    // clamp to a range of 0 to 1
    return (noise + 1.0) * 0.5f;
}

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
                    if (p.y != 0 && v.w == 0) modified.w = 2;
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
    m_position = Vec3(-5, -2, -3);

    std::random_device device;
    std::mt19937 generator(device());

    // procedurally generate the chunk
    int chunk_size = 16, chunk_height = 20;
    std::uniform_real_distribution<float> offset_dist(0, 1);

    // the smaller the frequency, the smoother the noise
    std::uniform_real_distribution<float> frequency_dist(0.05, 0.1);
    float frequency = frequency_dist(generator);

    for (int x = 0; x < chunk_size; x++) {
        for (int z = 0; z < chunk_size; z++) {
            float random_x = x + offset_dist(generator);
            float random_z = z + offset_dist(generator);
            Vec2 point(random_x * frequency, random_z * frequency);
            // draw bottom layers too
            int y = round(perlin_noise(point) * chunk_height);
            for (int depth = y; depth < chunk_height; depth++)
                m_voxels.insert({ Vec3(x, depth, z), true });
        }
    }

    compute_mesh();
    init_buffers();
}

void Chunk::render(ShaderManager& shaders)
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
}
