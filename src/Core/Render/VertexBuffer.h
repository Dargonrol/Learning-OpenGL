#pragma once
#include <array>
#include <vector>
#include "OpenGL.h"
#include "Renderer.h"

class VertexBuffer
{
public:
    VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);

    template<typename T>
    explicit VertexBuffer(std::vector<T>& data) : VertexBuffer(data.data(), static_cast<unsigned int>(data.size() * sizeof(T))) {}

    template<typename T, std::size_t N>
    explicit VertexBuffer(std::array<T, N>& data) : VertexBuffer(data.data(), static_cast<unsigned int>(N * sizeof(T))) {}

    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    VertexBuffer(VertexBuffer&& other) noexcept : m_RendererID(other.m_RendererID) { other.m_RendererID = 0; }
    VertexBuffer& operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other)
        {
            if (m_RendererID)
                GLCall(glDeleteBuffers(1, &m_RendererID))

            m_RendererID = other.m_RendererID;
            other.m_RendererID = 0;
        }
        return *this;
    }

    VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;


private:
    unsigned int m_RendererID{};
};
