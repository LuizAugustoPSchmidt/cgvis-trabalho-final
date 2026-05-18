#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture(const char* path, GLuint unit);
    ~Texture();

    void Bind() const;
    GLuint GetID() const { return m_ID; }
    GLuint GetUnit() const { return m_Unit; }

private:
    GLuint m_ID;
    GLuint m_Unit;
    GLuint m_SamplerID;
};

#endif // TEXTURE_H
