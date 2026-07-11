#pragma once

#include <glad/glad.h>
#include <vector>
#include <cassert>

struct VertexBufferElement
{
    uint32_t Type;
    uint32_t Count;
    unsigned char Normalized;

    static uint32_t GetSizeOfType(uint32_t type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return sizeof(float);
        case GL_UNSIGNED_INT:
            return sizeof(uint32_t);
        case GL_UNSIGNED_BYTE:
            return sizeof(unsigned char);
        }

        assert(false);
        return 0;
    }
};

struct VertexBufferLayout
{
    VertexBufferLayout()
        : m_Stride(0) {}

    template <typename T>
    void Push(uint32_t count)
    {
        static_assert(sizeof(T) == 0, "Unsupported VertexBufferLayout push type!");
    }

    inline const std::vector<VertexBufferElement> &GetElements() const { return m_Elements; }
    inline uint32_t GetStride() const { return m_Stride; }

private:
    std::vector<VertexBufferElement> m_Elements;
    uint32_t m_Stride;
};

template <>
inline void VertexBufferLayout::Push<float>(uint32_t count)
{
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template <>
inline void VertexBufferLayout::Push<uint32_t>(uint32_t count)
{
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(uint32_t count)
{
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
