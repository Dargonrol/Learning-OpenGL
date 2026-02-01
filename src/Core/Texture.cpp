#include "Texture.h"

#include <iostream>

#include "stb_image/stb_image.h"
#include "Renderer.h"
#include "../OpenGL.h"

Texture::Texture(const std::filesystem::path& path) : m_RendererID(0), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    if (!m_LocalBuffer)
    {
        std::cerr << "Failed to load image: " << path << "\n";
        std::cerr << "Reason: " << stbi_failure_reason() << "\n";
        return;
    }

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
    if (m_LocalBuffer)
    {
        stbi_image_free(m_LocalBuffer);
    }
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind()
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
