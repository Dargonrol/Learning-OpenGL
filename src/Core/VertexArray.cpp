#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "../OpenGL.h"

static GLenum ShaderDataTypeToOpenGL(const ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float: return GL_FLOAT;
        case ShaderDataType::uInt:  return GL_UNSIGNED_INT;
        case ShaderDataType::uByte: return GL_UNSIGNED_BYTE;
    }
    ASSERT(false);
    return 0;
}

VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) const
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(
            i,
            element.count,
            ShaderDataTypeToOpenGL(element.type),
            element.normalized,
            layout.GetStride(),
            reinterpret_cast<const void*>(offset)
            ))
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

