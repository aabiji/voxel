#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <vector>

#include "math.h"

class Shader
{
public:
    Shader();
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use();
    void assemble();
    void add(GLenum type, const char* path);

    void set_int(const char* name, int value) const;
    void set_matrix(const char* name, math::Matrix<4, 4>& matrix) const;
private:
    unsigned int m_program;
    std::vector<unsigned int> m_shaders;
};
