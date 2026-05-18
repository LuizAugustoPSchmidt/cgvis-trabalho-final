#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <string>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Use() const;
    GLuint GetID() const { return m_ID; }

    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    GLint GetUniformLocation(const std::string& name) const;

private:
    GLuint m_ID;

    void CheckCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H
