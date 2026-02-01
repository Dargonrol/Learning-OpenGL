#pragma once
#include "glad/gl.h"

class IndexBuffer
{
public:
    IndexBuffer(const GLuint* data, GLuint count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    [[nodiscard]] inline GLuint GetCount() const { return m_Count; }

private:
    GLuint m_RendererID;
    GLuint m_Count;
};
