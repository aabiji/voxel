#pragma once

#include <vector>

#include "math.h"
#include "utils.h"

class ShaderManager
{
public:
    ~ShaderManager();

    Result add_shader(int type, std::string path);
    Result assemble();

    void use();
    void set_vec3(std::string name, Vec3& value);
    void set_matrix4(std::string name, Matrix4& value);
private:
    unsigned int m_program;
    std::vector<unsigned int> m_shader_ids;
};