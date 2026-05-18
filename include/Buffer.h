#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>

class VertexBuffer {
public:
    VertexBuffer(const void* data, GLuint size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    GLuint m_ID;
};

class IndexBuffer {
public:
    IndexBuffer(const void* data, GLuint count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    GLuint GetCount() const { return m_Count; }

private:
    GLuint m_ID;
    GLuint m_Count;
};

#endif // BUFFER_H
