#pragma once
#include <cstdint>
#include <vector>

// Forward declarations
class IndexBuffer;
class VertexBuffer;
class VertexArray;
struct Vertex;
// --------------------

struct Mesh
{
    VertexArray VAO;
    VertexBuffer VBO;
    IndexBuffer IBO;

    Mesh() = default;
    ~Mesh() = default;

    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};


struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};