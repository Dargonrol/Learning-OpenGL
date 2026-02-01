#pragma once

#include <string>
#include <filesystem>

class Texture
{
public:
    explicit Texture(const std::filesystem::path& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind();

    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

private:
    unsigned int m_RendererID;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
};