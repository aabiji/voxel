#include <fstream>
#include <vector>
#include <string>

#include <glad/glad.h>

#include "shader.h"

ShaderManager::~ShaderManager() { glDeleteProgram(m_program); }

Result ShaderManager::load(std::string vertex_path, std::string fragment_path)
{
    auto result = add_shader(GL_VERTEX_SHADER, vertex_path);
    if (result.is_err()) return result;

    result = add_shader(GL_FRAGMENT_SHADER, fragment_path);
    if (result.is_err()) return result;

    return assemble();
}

void ShaderManager::use() { glUseProgram(m_program); }

void ShaderManager::set_vec3(std::string name, Vec3& value)
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(location, 1, &value.x);
}

void ShaderManager::set_int(std::string name, int value)
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(location, value);
}

void ShaderManager::set_matrix4(std::string name, Matrix4& value)
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, value.values);
}

ResultOr<std::string> read_file(std::string& path)
{
    std::ifstream file;
    file.open(path);
    if (!file.is_open())
        return Result("Failed to open {}", path);

    std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    file.close();
    return content;
}

Result ShaderManager::add_shader(int type, std::string path)
{
    auto result = read_file(path);
    if (result.is_err())
        return result.error();

    std::string source = result.value();
    const char* source_ptr = source.c_str();
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source_ptr, NULL);
    glCompileShader(shader);

    int success;
    char info_msg[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_msg);
        return Result("SHADER ERROR ({}): {}", path, info_msg);
    }

    m_shader_ids.push_back(shader);
    return {};
}

Result ShaderManager::assemble()
{
    m_program = glCreateProgram();
    for (unsigned int shader : m_shader_ids) {
        glAttachShader(m_program, shader);
    }
    glLinkProgram(m_program);

    int success;
    char info_msg[512];
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 512, NULL, info_msg);
        return Result("SHADER ERROR: {}", info_msg);
    }

    for (unsigned int shader : m_shader_ids) {
        glDeleteShader(shader);
    }
    m_shader_ids.clear();
    return {};
}
