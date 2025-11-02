#include <cstring>
#include <format>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "spritesheet.h"

Spritesheet::~Spritesheet() { glDeleteTextures(1, &m_texture); }

void Spritesheet::bind(ShaderManager& shaders, int unit)
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    shaders.set_int(std::format("texture{}", unit), unit);
}

void Spritesheet::load(const char* path, int sprite_size, int num_sprites)
{
    // create the texture
    int width, height, channels;
    unsigned char* spritesheet_pixels =
        stbi_load(path, &width, &height, &channels, 4);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, sprite_size, sprite_size, num_sprites);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    int x = 0, y = 0;
    for (int i = 0; i < num_sprites; i++) {
        // crop the specific sprite out of the spritesheet
        unsigned char* sprite_pixels =
            new unsigned char[sprite_size * sprite_size * channels];
        for (int row = 0; row < sprite_size; row++) {
            int src_index = y * sprite_size * channels;
            int dest_index = ((y + row) * width + x) * channels;
            int stride = sprite_size * channels * sizeof(unsigned char);
            std::memcpy(sprite_pixels + src_index, spritesheet_pixels + dest_index, stride);
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
