#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader
{
public:
    ~Shader() = default;
    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use();
    void cleanup();
    void assemble();
    void add(GLenum type, const char* path);

    void set_int(const char* name, int value) const;
    void set_matrix(const char* name, glm::mat4& value) const;
private:
    unsigned int m_program;
    std::vector<unsigned int> m_shaders;
};
