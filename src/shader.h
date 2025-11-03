#pragma once

#include <vector>

#include "math.h"
#include "utils.h"

class ShaderManager
{
public:
    ~ShaderManager();

    void use();
    void set_vec3(std::string name, Vec3& value);
    void set_int(std::string name, int value);
    void set_matrix4(std::string name, Matrix4& value);
    Result load(std::string vertex_path, std::string fragment_path);
private:
    Result add_shader(int type, std::string path);
    Result assemble();

    unsigned int m_program;
    std::vector<unsigned int> m_shader_ids;
};