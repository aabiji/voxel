#pragma once

#include <memory>

#include "chunk.h"

struct VoxelLocation {
    float chunk_x, chunk_z;
    float voxel_x, voxel_z;
};

class Terrain {
public:
    Terrain() { }

    VoxelLocation voxel_location(float x, float z)
    {
        float chunk_x = floor(x / float(CHUNK_SIZE));
        float chunk_z = floor(z / float(CHUNK_SIZE));
        float voxel_x = floor(x - chunk_x * CHUNK_SIZE);
        float voxel_z = floor(z - chunk_z * CHUNK_SIZE);
        return {
            .chunk_x = chunk_x, .chunk_z = chunk_z, .voxel_x = voxel_x, .voxel_z = voxel_z
        };
    }

    float surface_y(float x, float z)
    {
        VoxelLocation l = voxel_location(x, z);
        auto chunk = m_chunks.find(Vec3(l.chunk_x, 0, l.chunk_z));
        return chunk != m_chunks.end()
            ? chunk->second->get_surface_y(l.voxel_x, l.voxel_z)
            : -1;
    }

    bool voxel_exists(float x, float y, float z)
    {
        VoxelLocation l = voxel_location(x, z);
        auto chunk = m_chunks.find(Vec3(l.chunk_x, 0, l.chunk_z));
        return chunk != m_chunks.end()
            ? chunk->second->voxel_present(Vec3(l.voxel_x, y, l.voxel_z))
            : false;
    }

    // check if an object is colliding with any voxels
    bool collision(Vec3 position, Vec3 size, float ground_offset)
    {
        const float epsilon = 0.001; // prevents edge alignment bugs

        int min_x = std::floor(position.x);
        int max_x = std::floor(position.x + size.x - epsilon);

        int min_y = std::floor(position.y + ground_offset);
        int max_y = std::floor(position.y + size.y + ground_offset - epsilon);

        int min_z = std::floor(position.z);
        int max_z = std::floor(position.z + size.z - epsilon);

        for (int x = min_x; x <= max_x; x++) {
            for (int y = min_y; y <= max_y; y++) {
                for (int z = min_z; z <= max_z; z++) {
                    if (voxel_exists(x, y, z))
                        return true;
                }
            }
        }

        return false;
    }

    void load_more_chunks(float pos_x, float pos_z)
    {
        // create new chunks around the current chunk continuously
        const int radius = 2;
        VoxelLocation l = voxel_location(pos_x, pos_z);

        for (int x = -radius; x <= radius; x += 1) {
            for (int z = -radius; z <= radius; z++) {
                Vec3 chunk_pos(l.chunk_x + x, 0, l.chunk_z + z);
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
