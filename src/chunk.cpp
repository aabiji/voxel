#include <assert.h>
#include <glad/glad.h>
#include <random>

#include "chunk.h"
#include "utils.h"

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
inline float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }
struct Vec2 { float x, y; };

Vec2 gradient(int x, int y)
{
    // xxHash
    int h = x * 3266489917 + y * 668265263;
    h ^= h >> 15;
    h *= 2246822519;
    h ^= h >> 13;
    h *= 3266489917;
    h ^= h >> 16;
    // random gradient vector
    Vec2 directions[] = {
        Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1},
        Vec2{1, 1}, Vec2{-1, 1}, Vec2{1, -1}, Vec2{-1, -1}
    };
    return directions[h & 7];
}

float perlin_noise(float x, float y)
{
    // get the grid cell the point's in and
    // the direction of the point in that grid cell
    int X = std::floor(x);
    int Y = std::floor(y);
    float dx = x - X;
    float dy = y - Y;

    // get the gradient vectors for each corner
    Vec2 gtl = gradient(X, Y);
    Vec2 gtr = gradient(X + 1, Y);
    Vec2 gbl = gradient(X, Y + 1);
    Vec2 gbr = gradient(X + 1, Y + 1);

    // compute dot products to get the gradient values for each corner
    float vtl = gtl.x * dx       + gtl.y * dy;
    float vtr = gtr.x * (dx - 1) + gtr.y * dy;
    float vbl = gbl.x * dx       + gbl.y * (dy - 1);
    float vbr = gbr.x * (dx - 1) + gbr.y * (dy - 1);

    // interpolate those values
    float a = lerp(vtl, vtr, fade(dx));
    float b = lerp(vbl, vbr, fade(dx));
    float noise = lerp(a, b, fade(dy));

    // normalize to a range of 0 to 1
    return noise * 0.7f + 0.5f;
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
            float noise = perlin_noise(random_x * frequency, random_z * frequency);
            // draw bottom layers too
            int y = round(noise * chunk_height);
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
