#version 460 core

layout (location = 0) in vec3 input_pos;
layout (location = 1) in vec2 in_texture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texture_coord;

void main()
{
    gl_Position = projection * view * model * vec4(input_pos, 1.0);
    texture_coord = in_texture_coord;
}
