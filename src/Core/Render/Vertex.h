#pragma once

#include <glm/vec2.hpp>

#include "VertexBufferLayout.h"
#include "glm/vec3.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    static VertexBufferLayout GetLayout()
    {
        VertexBufferLayout layout;
a
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);

        return layout;
    }
};
