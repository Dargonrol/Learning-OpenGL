#pragma once

#include <filesystem>
#include <stb_image.h>
#include <iostream>

#include "renderer/Renderer.h"
#include "core/OpenGL.h"
#include "core/Util.h"

struct Texture {

    unsigned int m_RendererID = 0;

    Texture() : m_RendererID(0) {}

    explicit Texture(const std::filesystem::path& path)
    {
        int m_Width, m_Height, m_BPP;
        unsigned char* m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

        if (!m_LocalBuffer)
        {
            std::cerr << "Failed to load image: " << path << "\n";
            std::cerr << "Reason: " << stbi_failure_reason() << "\n";
            m_RendererID = 0;
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

        stbi_image_free(m_LocalBuffer);
    }

    ~Texture()
    {
        if (m_RendererID != 0)
            GLCall(glDeleteTextures(1, &m_RendererID));
    }

    void Bind(unsigned int slot) const
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + slot));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    }

    void Unbind()
    {
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    }

    Texture(Texture&& other) noexcept : m_RendererID(other.m_RendererID) { other.m_RendererID = 0; }
    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            if (m_RendererID != 0)
                GLCall(glDeleteTextures(1, &m_RendererID));

            m_RendererID = other.m_RendererID;
            other.m_RendererID = 0;
        }
        return *this;
    }

    // copy would trigger destructor which frees tex on gpu.
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};
