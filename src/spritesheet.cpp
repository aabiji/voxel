#include <vector>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "spritesheet.h"

void Spritesheet::load(const char* path, int num_sprites, int sprite_size)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels; // requesting channels to always be 4
    unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4);
    if (pixels == nullptr)
        throw std::runtime_error("Failed to open " + std::string(path));

    int max_layers = 0;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
    if (num_sprites >= max_layers) {
        stbi_image_free(pixels);
        throw std::runtime_error("Too many sprites");
    }

    // alloate the texture array
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, sprite_size, sprite_size, num_sprites);

    int x = 0, y = 0;
    for (int i = 0; i < num_sprites; i++) {
        // extract the sprite from the overall image
        std::vector<unsigned char> sprite;
        sprite.reserve(sprite_size * sprite_size * channels);
        for (int row = 0; row < sprite_size; row++) {
            int start = (y + row) * (width * channels) + (x * channels);
            int end = start + sprite_size * channels;
            sprite.insert(sprite.end(), pixels + start, pixels + end);
        }

        // upload the sprite data
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, sprite_size, sprite_size,
            1, GL_RGBA, GL_UNSIGNED_BYTE, sprite.data());
        x += sprite_size;
        if (x >= width) {
            x = 0;
            y += sprite_size;
        }
    }
    stbi_image_free(pixels);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}
