#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 coord;
layout (location = 2) in vec3 world_pos;

uniform mat4 view;
uniform mat4 projection;

flat out vec3 current_world_pos;

out vec2 uv;
flat out int texture_index;

void main()
{
    // vertex positions are already transformed during chunk mesh generation
    current_world_pos = world_pos;
    gl_Position = projection * view * vec4(pos, 1.0);

    uv = coord.xy;
    texture_index = int(coord.z);
}
