#pragma once

#include <memory>
#include <vector>

#include "vertex.h"
#include "utils.h"

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

class Terrain
{
public:
    Terrain()
    {
        auto chunk = std::make_shared<Chunk>(Vec3(0, 0, 0));
        m_chunks.insert({ Vec3(0, 0, 0), chunk });
    }

    // Get the y value for the voxel at the position in the world
    // return an error if no chunk is found
    ResultOr<float> get_surface_y(Vec3 position)
    {
        float chunk_x = floor(position.x / float(CHUNK_SIZE));
        float chunk_z = floor(position.z / float(CHUNK_SIZE));
        float voxel_x = floor(position.x - chunk_x * CHUNK_SIZE);
        float voxel_z = floor(position.z - chunk_z * CHUNK_SIZE);

        auto chunk = m_chunks.find(Vec3(chunk_x, 0, chunk_z));
        if (chunk == m_chunks.end())
            return Result("chunk not found");
        return chunk->second->get_surface_y(voxel_x, voxel_z);
    }

    void render()
    {
        for (const auto& [_, chunk] : m_chunks)
            chunk->render();
    }
private:
    std::unordered_map<Vec3, std::shared_ptr<Chunk>, Vec3Hasher> m_chunks;
};
