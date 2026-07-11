#include <glad/glad.h>

#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void *data, uint32_t size)
{
  glGenBuffers(1, &m_BufferID);
  glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
  glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::Bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::Unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
