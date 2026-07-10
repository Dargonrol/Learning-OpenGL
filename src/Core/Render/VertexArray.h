#pragma once
#include "Core/Util.h"
#include "OpenGL.h"
#include "Renderer.h"

class VertexBufferLayout;
class VertexBuffer;

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;
    void Bind() const;
    void Unbind() const;

    VertexArray(VertexArray&& other) noexcept : m_RendererID(other.m_RendererID) { other.m_RendererID = 0; }
    VertexArray& operator=(VertexArray&& other) noexcept
    {
        if (this != &other)
        {
            if (m_RendererID)
                GLCall(glDeleteVertexArrays(1, &m_RendererID))

            m_RendererID = other.m_RendererID;
            other.m_RendererID = 0;
        }
        return *this;
    }

    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;

private:
    unsigned int m_RendererID{};
};