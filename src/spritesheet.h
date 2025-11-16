#pragma once

#include "shader.h"

class Spritesheet {
public:
    ~Spritesheet();
    Result load(const char* path, int sprite_size, int num_sprites);
    void bind(ShaderManager& shaders, int unit);

private:
    unsigned int m_texture;
};
