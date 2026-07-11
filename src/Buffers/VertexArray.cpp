#include <glad/glad.h>

#include "VertexArray.h"

VertexArray::VertexArray()
{
  glGenVertexArrays(1, &m_VAO);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &m_VAO);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, VertexBufferLayout &layout)
{
  Bind();
  vb.Bind();

  const auto &elements = layout.GetElements();
  uint32_t offset = 0;
  for(uint32_t i = 0; i < elements.size(); i++)
  {
    const auto &element = elements[i];
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, element.Count, element.Type, element.Normalized , layout.GetStride(), (const void*) (uintptr_t)offset);
    offset += element.Count * VertexBufferElement::GetSizeOfType(element.Type);
  }
}

void VertexArray::Bind() const
{
  glBindVertexArray(m_VAO);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}
