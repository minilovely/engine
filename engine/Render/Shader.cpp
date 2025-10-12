#include "Shader.h"
#include "glad.h"
#include <iostream>

static bool CheckCompile(unsigned int shader)
{
    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << "Shader compile error:\n" << log << std::endl;
    }
    return ok;
}

static bool CheckLink(unsigned int prog)
{
    int ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        std::cout << "Program link error:\n" << log << std::endl;
    }
    return ok;
}

Shader::Shader(const std::string& vs_src, const std::string& fs_src)
{
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    const char* c_vs = vs_src.c_str();
    glShaderSource(vs, 1, &c_vs, nullptr);
    glCompileShader(vs);
    CheckCompile(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* c_fs = fs_src.c_str();
    glShaderSource(fs, 1, &c_fs, nullptr);
    glCompileShader(fs);
    CheckCompile(fs);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    CheckLink(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::use() const
{
    glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::setInt(const std::string& name, int i) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), i);
}

void Shader::setFloat(const std::string& name, float f) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), f);
}


