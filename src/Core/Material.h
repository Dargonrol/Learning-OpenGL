#pragma once
#include "AssetPool.h"
#include "glm/vec3.hpp"
#include <filesystem>

// fwd declares
class Shader;
class ResourceManager;
// fwd declares

struct Material
{
    Handle shaderHandle = {0, 0};
    glm::vec3 ambient = {1.0f, 1.0f, 1.0f};     // ambient lighting color
    glm::vec3 diffuse = {1.0f, 1.0f, 1.0f};     // color of surface under diffuse lighting
    glm::vec3 specular = {1.0f, 1.0f, 1.0f};    // color of specular highlight
    float shininess = 0.5f;

    static Handle parseMaterial(std::string_view name, const std::filesystem::path& path, ResourceManager& rm, int& error);

    friend std::ostream& operator<<(std::ostream &os, const Material& mat)
    {
        os << "Material:\n Shader " << mat.shaderHandle << "\n" <<
            "Ambient: {" << mat.ambient.x << ", " << mat.ambient.y << ", " << mat.ambient.z << "}\n" <<
            "diffuse: {" << mat.diffuse.x << ", " << mat.diffuse.y << ", " << mat.diffuse.z << "}\n" <<
            "specular: {" << mat.specular.x << ", " << mat.specular.y << ", " << mat.specular.z << "}\n" <<
            "shininess: " << mat.shininess;
        return os;
    }
};
