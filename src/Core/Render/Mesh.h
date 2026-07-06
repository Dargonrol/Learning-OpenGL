#pragma once

class IndexBuffer;
class VertexBuffer;
class VertexArray;

struct Mesh {
    VertexArray VAO;
    VertexBuffer VBO;
    IndexBuffer  IBO;
};