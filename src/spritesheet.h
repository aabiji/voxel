#pragma once

#include "shader.h"

class Spritesheet
{
public:
    Spritesheet() {}
    ~Spritesheet() { glDeleteTextures(1, &m_texture); }
    Spritesheet(const Spritesheet&) = delete;
    Spritesheet& operator=(const Spritesheet&) = delete;

    void load(const char* path, int num_sprites, int sprite_size);

    void set_unit(Shader& shader, const char* name, int unit)
    {
        shader.use();
        glActiveTexture(GL_TEXTURE0 + unit);
        shader.set_int(name, unit);
        m_unit = unit;
    }

    void bind()
    {
        glActiveTexture(GL_TEXTURE0 + m_unit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);
    }
private:
    unsigned int m_texture, m_unit;
};
