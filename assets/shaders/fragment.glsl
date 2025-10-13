#version 460 core

in vec2 texture_coord;

uniform sampler2D texture1;

out vec4 fragment_color;

void main()
{
    fragment_color = texture(texture1, texture_coord);
}
