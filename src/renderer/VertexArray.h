#pragma once
#include "core/Util.h"
#include "core/OpenGL.h"
#include "renderer/Renderer.h"

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

    unsigned int GetRendererID() const;

    static void BindVertexArray(unsigned int ID);

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

    bool operator==(const VertexArray& other) const
    {
        if (this->m_RendererID == other.m_RendererID)
            return true;
        return false;
    }

private:
    unsigned int m_RendererID{};
};