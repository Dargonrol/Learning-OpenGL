#pragma once
#include "AssetPool.h"
#include "glm/vec3.hpp"

// fwd declares
class Shader;
// fwd declares

struct Material
{
    Handle shaderHandle = {0, 0};
    glm::vec3 ambient = {1.0f, 1.0f, 1.0f};     // ambient lighting color
    glm::vec3 diffuse = {1.0f, 1.0f, 1.0f};     // color of surface under diffuse lighting
    glm::vec3 specular = {1.0f, 1.0f, 1.0f};    // color of specular highlight
    float shininess = 0.5f;
};

// material parser

