#pragma once
#include <array>
#include <vector>

class VertexBuffer
{
public:
    VertexBuffer(const void* data, unsigned int size);

    template<typename T>
    VertexBuffer(std::vector<T>& data) : VertexBuffer(data.data(), static_cast<unsigned int>(data.size() * sizeof(T))) {}

    template<typename T, std::size_t N>
    VertexBuffer(std::array<T, N>& data) : VertexBuffer(data.data(), static_cast<unsigned int>(N * sizeof(T))) {}

    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_RendererID{};
};
