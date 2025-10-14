#include <fstream>
#include "shader.h"

Shader::Shader() { m_program = glCreateProgram(); }

Shader::~Shader() { if (m_program) glDeleteProgram(m_program);  }

void Shader::use() { if (m_program) glUseProgram(m_program); }

void Shader::set_int(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name), value);
}

void Shader::set_matrix(const char* name, math::Matrix<4, 4>& matrix) const
{
    int location = glGetUniformLocation(m_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.ptr());
}

void Shader::add(GLenum type, const char* path)
{
    // read the file
    std::ifstream file(path);
    if (!file.is_open() || !file.good())
        throw std::runtime_error("Failed to open " + std::string(path));

    std::string source = "", line = "";
    while (std::getline(file, line))
        source += line + '\n'; 
    file.close();

    // compile the shader
    const char* str = source.c_str();
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &str, nullptr);
    glCompileShader(shader);

    // check for errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, info);
        glDeleteShader(shader);
        throw std::runtime_error(std::string(info));
    }

    m_shaders.push_back(shader);
}

void Shader::assemble()
{
    // assemble and link the program
    for (size_t i = 0; i < m_shaders.size(); i++)
        glAttachShader(m_program, m_shaders[i]);
    glLinkProgram(m_program);
    for (size_t i = 0; i < m_shaders.size(); i++)
        glDeleteShader(m_shaders[i]);

    // check for errors
    int success;
    char info[1024];
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 1024, nullptr, info);
        glDeleteProgram(m_program);
        throw std::runtime_error(std::string(info));
    }
}
