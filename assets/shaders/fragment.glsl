#version 460 core

in vec2 uv;
flat in int texture_index;
uniform sampler2DArray textures;

flat in vec3 current_world_pos;
uniform vec3 selected_world_pos;

out vec4 fragment_color;

void main()
{
    // inner or outer edge
    float threshold = 0.02;
    bool is_edge = min(uv.x, uv.y) < threshold || max(uv.x, uv.y) > (1.0 - threshold);
    bool highlight = is_edge && current_world_pos == selected_world_pos;
    if (highlight)
        fragment_color = vec4(1, 1, 1, 1);
    else
        fragment_color = texture(textures, vec3(uv, texture_index));
}
