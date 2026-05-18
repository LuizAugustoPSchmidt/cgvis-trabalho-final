#include "VertexArray.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::Bind() const {
    glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(const VertexBuffer& vbo, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
    Bind();
    vbo.Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void VertexArray::SetIndexBuffer(const IndexBuffer& ibo) {
    Bind();
    ibo.Bind();
}
