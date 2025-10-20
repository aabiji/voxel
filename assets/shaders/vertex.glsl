#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 coord;

layout (std430, binding = 0) buffer VoxelData
{
    vec3 positions[5 * 5 * 5];
};

uniform mat4 view;
uniform mat4 projection;

out vec3 texture_coord;

void main()
{
    vec3 voxel_position = pos + positions[gl_InstanceID];
    gl_Position = projection * view * vec4(voxel_position, 1.0);
    texture_coord = coord;
}
