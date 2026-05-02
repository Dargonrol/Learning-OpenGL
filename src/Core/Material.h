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
    Handle diffuseMap = {0, 0};
    Handle specularMap = {0, 0};
    glm::vec3 ambient = {1.0f, 1.0f, 1.0f};     // ambient lighting color
    glm::vec3 diffuse = {1.0f, 1.0f, 1.0f};     // color of surface under diffuse lighting
    glm::vec3 specular = {0.0f, 0.0f, 0.0f};    // color of specular highlight
    float shininess = 0.5f;

    void BindShader(ResourceManager& rm) const;

    static Handle parseMaterial(std::string_view name, const std::filesystem::path& path, ResourceManager& rm, int& error, bool replace = false);

    friend std::ostream& operator<<(std::ostream &os, const Material& mat)
    {
        os << "Material:\n Shader " << mat.shaderHandle << "\n" <<
            "diffuse Map " << mat.diffuseMap << "\n" <<
            "specular Map " << mat.specularMap << "\n" <<
            "Ambient: {" << mat.ambient.x << ", " << mat.ambient.y << ", " << mat.ambient.z << "}\n" <<
            "diffuse: {" << mat.diffuse.x << ", " << mat.diffuse.y << ", " << mat.diffuse.z << "}\n" <<
            "specular: {" << mat.specular.x << ", " << mat.specular.y << ", " << mat.specular.z << "}\n" <<
            "shininess: " << mat.shininess;
        return os;
    }

    enum class Tokens
    {
        SHADER_NAME,

        VERT_SHADER_PATH,
        FRAG_SHADER_PATH,
        TC_SHADER_PATH,
        TE_SHADER_PATH,
        GEO_SHADER_PATH,

        DIFF_MAP_PATH,
        SPEC_MAP_PATH,

        AMBIENT_VALUE,
        DIFFUSE_VALUE,
        SPECULAR_VALUE,
        SHININESS_VALUE
    };

    static inline const std::unordered_map<Tokens, std::string> tokenEnumStrMap = {
        {Tokens::SHADER_NAME, "shader_name"},
        {Tokens::VERT_SHADER_PATH, "vert_shader"},
        {Tokens::FRAG_SHADER_PATH, "frag_shader"},
        {Tokens::TC_SHADER_PATH, "tc_shader"},
        {Tokens::TE_SHADER_PATH, "te_shader"},
        {Tokens::GEO_SHADER_PATH, "geo_shader"},

        {Tokens::DIFF_MAP_PATH, "diffuse_map"},
        {Tokens::SPEC_MAP_PATH, "specular_map"},

        {Tokens::AMBIENT_VALUE, "ambient"},
        {Tokens::DIFFUSE_VALUE, "diffuse"},
        {Tokens::SPECULAR_VALUE, "specular"},
        {Tokens::SHININESS_VALUE, "shininess"}
    };
};
