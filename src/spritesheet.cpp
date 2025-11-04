#include <cstring>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "spritesheet.h"

Spritesheet::~Spritesheet() { glDeleteTextures(1, &m_texture); }

void Spritesheet::bind(ShaderManager& shaders, int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    shaders.set_int("texture" + std::to_string(unit), unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
}

void Spritesheet::load(const char* path, int sprite_size, int num_sprites)
{
    // create the texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, sprite_size, sprite_size, num_sprites);
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* spritesheet_pixels =
        stbi_load(path, &width, &height, &channels, 4);

    int x = 0, y = 0;
    for (int i = 0; i < num_sprites; i++) {
        // crop the specific sprite out of the spritesheet
        unsigned char* sprite_pixels =
            new unsigned char[sprite_size * sprite_size * 4];
        for (int row = 0; row < sprite_size; row++) {
            int dest_index = row * sprite_size * 4;
            int src_index = ((y + row) * width + x) * 4;
            int stride = sprite_size * 4;
            std::memcpy(sprite_pixels + dest_index, spritesheet_pixels + src_index, stride);
        }

        // upload the image
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
            sprite_size, sprite_size, 1,
            GL_RGBA, GL_UNSIGNED_BYTE, sprite_pixels);
        delete[] sprite_pixels;

        // advance to the next sprite
        x += sprite_size;
        if (x >= width) {
            x = 0;
            y += sprite_size;
        }
    }

    stbi_image_free(spritesheet_pixels);
}
