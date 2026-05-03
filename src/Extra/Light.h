#pragma once

//fwd delcare
#include <glm/vec3.hpp>

struct Light
{
    enum class LightType
    {
        DIRECTIONAL = 0,
        POINT = 1,
        SPOTLIGHT = 2
    };

    LightType lightType = LightType::POINT;

    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};