#pragma once
#include <cstdint>
#include <vector>

#include "renderer/Vertex.h"
#include "renderer/VertexBuffer.h"
#include "renderer/VertexArray.h"
#include "renderer/IndexBuffer.h"


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
        VertexBuffer VBO{vertices};

        VAO.AddBuffer(VBO, Vertex::GetLayout());

        IndexBuffer IBO{indices};

        return {std::move(VAO), std::move(VBO), std::move(IBO)};
    }
};