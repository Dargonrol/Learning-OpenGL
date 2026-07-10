#pragma once

#include <glm/vec2.hpp>

#include "VertexBufferLayout.h"
#include "glm/vec3.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;

    static VertexBufferLayout GetLayout()
    {
        VertexBufferLayout layout;

        layout.Push<float>(3);
        layout.Push<float>(2);
        layout.Push<float>(3);

        return layout;
    }
};
