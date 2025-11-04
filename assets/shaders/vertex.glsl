#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 coord;

uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
flat out int texture_index;

void main()
{
    gl_Position = projection * view * vec4(pos, 1.0);
    uv = coord.xy;
    texture_index = int(coord.z);
}
