#pragma once

#include <memory>

#include "chunk.h"
#include "utils.h"

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
    ResultOr<float> get_surface_y(float x, float z)
    {
        float chunk_x = floor(x / float(CHUNK_SIZE));
        float chunk_z = floor(z / float(CHUNK_SIZE));
        float voxel_x = floor(x - chunk_x * CHUNK_SIZE);
        float voxel_z = floor(z - chunk_z * CHUNK_SIZE);

        auto chunk = m_chunks.find(Vec3(chunk_x, 0, chunk_z));
        if (chunk == m_chunks.end())
            return Result("chunk not found");
        return chunk->second->get_surface_y(voxel_x, voxel_z);
    }

    bool voxel_collision(Vec3 position, Vec3 size)
    {
        return false; // TODO!
    }

    void load_more_chunks(float pos_x, float pos_z)
    {
        float chunk_x = floor(pos_x / float(CHUNK_SIZE));
        float chunk_z = floor(pos_z / float(CHUNK_SIZE));
        const int radius = 2;

        // create new chunks around the current chunk continuously
        for (int x = -radius; x <= radius; x += 1) {
            for (int z = -radius; z <= radius; z++) {
                Vec3 chunk_pos(chunk_x + x, 0, chunk_z + z);
                if (!m_chunks.count(chunk_pos)) {
                    auto chunk = std::make_shared<Chunk>(chunk_pos);
                    m_chunks.insert({ chunk_pos, chunk });
                }
            }
        }
    }

    void render()
    {
        for (const auto& [_, chunk] : m_chunks)
            chunk->render();
    }
private:
    std::unordered_map<Vec3, std::shared_ptr<Chunk>, Vec3Hasher> m_chunks;
};
