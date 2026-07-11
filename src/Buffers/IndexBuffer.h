#pragma once

#include <cstdint>

struct IndexBuffer
{
  IndexBuffer(const void *data, uint32_t i_count);
  virtual ~IndexBuffer();
  void Bind() const;
  void Unbind() const;

  inline uint32_t getCount() const {return m_Count;}

private:
  uint32_t m_IBO;
  uint32_t m_Count;
};
