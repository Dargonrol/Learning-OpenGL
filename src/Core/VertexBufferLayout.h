#pragma once
#include <cstdint>
#include <vector>

#include "Util.h"

enum class ShaderDataType
{
    Float,
    uInt,
    uByte
};

struct VertexBufferElement
{
    ShaderDataType type;
    unsigned int count;
    bool normalized;

    static unsigned int GetSizeOfType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:          return sizeof(float);
            case ShaderDataType::uInt:   return sizeof(std::uint32_t);
            case ShaderDataType::uByte:  return sizeof(std::uint8_t);
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : m_Stride(0) {}
    ~VertexBufferLayout() = default;

    template<typename T>
    void Push(unsigned int count);

    [[nodiscard]] std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
    [[nodiscard]] unsigned int GetStride() const { return m_Stride; }

private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
};

template<typename T>
    void VertexBufferLayout::Push(unsigned int count)
{
    static_assert(false);
}

template<>
inline void VertexBufferLayout::Push<float>(unsigned int count)
{
    m_Elements.push_back({ ShaderDataType::Float, count, false });
    m_Stride += VertexBufferElement::GetSizeOfType(ShaderDataType::Float) * count;
}

template<>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count)
{
    m_Elements.push_back({ ShaderDataType::uInt, count, false });
    m_Stride += VertexBufferElement::GetSizeOfType(ShaderDataType::uInt) * count;
}

template<>
inline void VertexBufferLayout::Push<unsigned char>(unsigned int count)
{
    m_Elements.push_back({ ShaderDataType::uByte, count, true });
    m_Stride += VertexBufferElement::GetSizeOfType(ShaderDataType::uByte) * count;
}