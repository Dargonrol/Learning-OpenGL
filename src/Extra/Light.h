#pragma once
#include "glm/vec3.hpp"

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOTLIGHT
};

struct Light
{
    LightType type;
    glm::vec3 position = {0,0,0};
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;


};