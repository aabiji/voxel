#version 460 core

in vec3 texture_coord;

uniform sampler2DArray textures;

out vec4 fragment_color;

void main()
{
    fragment_color = texture(textures, texture_coord);
}
