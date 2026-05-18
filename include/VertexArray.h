#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <glad/glad.h>
#include "Buffer.h"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const VertexBuffer& vbo, GLuint index, GLint size, GLenum type = GL_FLOAT, GLboolean normalized = GL_FALSE, GLsizei stride = 0, const void* pointer = 0);
    void SetIndexBuffer(const IndexBuffer& ibo);

private:
    GLuint m_ID;
};

#endif // VERTEXARRAY_H
