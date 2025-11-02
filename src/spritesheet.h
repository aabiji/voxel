#pragma once

#include "shader.h"

class Spritesheet
{
public:
    ~Spritesheet();
    void load(const char* path, int sprite_size, int num_sprites);
    void bind(ShaderManager& shaders, int unit);
private:
    unsigned int m_texture;
};