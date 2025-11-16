#pragma once

#include <array>
#include <unordered_map>

#include "math.h"

struct Vertex {
    // vertex position
    float x, y, z;
    // texture coordinates
    // w is the index into the cube spritesheet texture array
    float u, v, w;
};

// map a face direction to a the triangle vertices that make up that face
using VoxelFaces = std::unordered_map<Vec3, std::array<Vertex, 4>, Vec3Hasher>;
inline VoxelFaces get_voxel_faces()
{
    VoxelFaces voxel_faces;
    voxel_faces[Vec3(1, 0, 0)] = { // right face
        Vertex { 0.5, -0.5, -0.5, 1.0f, 0.0, 0.0 },
        Vertex { 0.5, 0.5, -0.5, 1.0f, 1.0, 0.0 },
        Vertex { 0.5, 0.5, 0.5, 0.0f, 1.0, 0.0 },
        Vertex { 0.5, -0.5, 0.5, 0.0f, 0.0, 0.0 }
    };

    voxel_faces[Vec3(-1, 0, 0)] = { // left face
        Vertex { -0.5, -0.5, -0.5, 0.0, 0.0, 0.0 },
        Vertex { -0.5, -0.5, 0.5, 1.0, 0.0, 0.0 },
        Vertex { -0.5, 0.5, 0.5, 1.0, 1.0, 0.0 },
        Vertex { -0.5, 0.5, -0.5, 0.0, 1.0, 0.0 }
    };

    voxel_faces[Vec3(0, 1, 0)] = { // top face
        Vertex { -0.5, 0.5, -0.5, 0.0, 1.0, 1.0 },
        Vertex { -0.5, 0.5, 0.5, 0.0, 0.0, 1.0 },
        Vertex { 0.5, 0.5, 0.5, 1.0, 0.0, 1.0 },
        Vertex { 0.5, 0.5, -0.5, 1.0, 1.0, 1.0 }
    };

    voxel_faces[Vec3(0, -1, 0)] = { // bottom face
        Vertex { -0.5, -0.5, -0.5, 0.0, 0.0, 2.0 },
        Vertex { 0.5, -0.5, -0.5, 1.0, 0.0, 2.0 },
        Vertex { 0.5, -0.5, 0.5, 1.0, 1.0, 2.0 },
        Vertex { -0.5, -0.5, 0.5, 0.0, 1.0, 2.0 }
    };

    voxel_faces[Vec3(0, 0, 1)] = { // front face
        Vertex { -0.5, -0.5, 0.5, 0.0, 0.0, 0.0 },
        Vertex { 0.5, -0.5, 0.5, 1.0, 0.0, 0.0 },
        Vertex { 0.5, 0.5, 0.5, 1.0, 1.0, 0.0 },
        Vertex { -0.5, 0.5, 0.5, 0.0, 1.0, 0.0 }
    };

    voxel_faces[Vec3(0, 0, -1)] = { // back face
        Vertex { -0.5, -0.5, -0.5, 1.0, 0.0, 0.0 },
        Vertex { -0.5, 0.5, -0.5, 1.0, 1.0, 0.0 },
        Vertex { 0.5, 0.5, -0.5, 0.0, 1.0, 0.0 },
        Vertex { 0.5, -0.5, -0.5, 0.0, 0.0, 0.0 }
    };
    return voxel_faces;
}
