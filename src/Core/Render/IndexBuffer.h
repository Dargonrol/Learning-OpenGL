#pragma once
#include <vector>

#include "Core/Util.h"
#include "Renderer.h"
#include "OpenGL.h"

class IndexBuffer
{
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    IndexBuffer(const std::vector<unsigned int> &data) : IndexBuffer(data.data(), static_cast<unsigned int>(data.size())) {}
    template<std::size_t N>
    IndexBuffer(const std::array<unsigned int, N> &data) : IndexBuffer(data.data(), static_cast<unsigned int>(N)) {}
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    [[nodiscard]] inline unsigned int GetCount() const { return m_Count; }

    IndexBuffer(IndexBuffer&& other) noexcept : m_RendererID(other.m_RendererID) { other.m_RendererID = 0; other.m_Count = 0; }
    IndexBuffer& operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            if (m_RendererID)
                GLCall(glDeleteBuffers(1, &m_RendererID))

            m_RendererID = other.m_RendererID;
            m_Count = other.m_Count;

            other.m_RendererID = 0;
            other.m_Count = 0;
        }
        return *this;
    }

    IndexBuffer(const IndexBuffer& other) = delete;
    IndexBuffer& operator=(const IndexBuffer& other) = delete;

private:
    unsigned int m_RendererID{};
    unsigned int m_Count{};
};
