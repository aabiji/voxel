#version 460 core

in vec2 uv;
flat in int texture_index;

uniform sampler2DArray textures;

out vec4 fragment_color;

void main()
{
    fragment_color = texture(textures, vec3(uv, texture_index));
}
