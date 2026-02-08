#pragma once
#include <vector>

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

private:
    unsigned int m_RendererID{};
    unsigned int m_Count;
};
