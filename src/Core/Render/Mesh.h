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
    VertexArray     VAO;
    VertexBuffer    VBO;
    IndexBuffer     IBO;

    Mesh() = default;
    Mesh(VertexArray&& vao, VertexBuffer&& vbo, IndexBuffer&& ibo) : VAO(std::move(vao)), VBO(std::move(vbo)), IBO(std::move(ibo)) {}

    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
};


struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh UploadToGPU()
    {
        if (vertices.empty() || indices.empty())
            return {};

        VertexArray VAO{};
        VertexBuffer VBO{vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(Vertex))};

        VAO.AddBuffer(VBO, Vertex::GetLayout());

        IndexBuffer IBO{indices.data(),  static_cast<unsigned int>(indices.size())};

        return {std::move(VAO), std::move(VBO), std::move(IBO)};
    }
};