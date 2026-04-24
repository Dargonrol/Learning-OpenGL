#include "Material.h"

#include <fstream>
#include <iostream>

#include "Parser.h"
#include "ResourceManager.h"
#include "Util.h"

Handle handleShader(std::string_view shaderName, const std::filesystem::path& shaderPath, Shader::Type type, ResourceManager& rm, int& error)
{
    if (rm.shaderPool.Exists(shaderName))
        return rm.shaderPool.GetHandle(shaderName);

    std::filesystem::path absolutePath;
    if (shaderPath.is_relative())
        absolutePath = BASE_PATH / shaderPath;
    else
        absolutePath = shaderPath;

    auto shader = std::make_unique<Shader>(absolutePath, type, error);
    if (error)
    {
        std::cerr << "Error parsing material with shader: " << shaderPath << std::endl;
        return {0, 0};
    }

    if (shaderName.empty())
        shaderName = shaderPath.filename().c_str();

    return rm.shaderPool.ReplaceData(shaderName, std::move(shader));
}

Handle handleTexMaps(const std::filesystem::path& path, ResourceManager& rm, int slot = 0)
{
    if (path.empty())
        return {0,0};
    std::filesystem::path absolutePath;
    if (path.is_relative())
        absolutePath = BASE_PATH / path;
    else
        absolutePath = path;

    auto texture = std::make_unique<Texture>(absolutePath);
    return rm.texturePool.ReplaceData(absolutePath.filename().c_str(), std::move(texture));
}


template<typename T, typename Variant>
struct is_in_variant;

template<typename T, typename... Ts>
struct is_in_variant<T, std::variant<Ts...>>
    : std::disjunction<std::is_same<T, Ts>...> {};

template<typename T, typename Variant>
inline constexpr bool is_in_variant_v = is_in_variant<T, Variant>::value;

template<typename T>
requires is_in_variant_v<T, Parser::Value>
bool assignValue(const std::string_view& key, const std::string_view& key2, const std::variant<Parser::Value>& value, T& target)
{
    if (key == key2)
    {
        if (const T* p = std::get_if<T>(&value))
        {
            target = *p;
            return true;
        }
    }
    return false;
}


Handle Material::parseMaterial(const std::string_view name, const std::filesystem::path &path, ResourceManager& rm, int& error, bool replace)
{
    if (!replace)
        if (rm.materialPool.Exists(name))
            return rm.materialPool.GetHandle(name);

    auto material = std::make_unique<Material>();

    Parser parser(path);
    parser.Parse();

    std::string_view vertShaderPath;
    std::string_view vertShaderName;
    std::string_view diffuseMapPath;
    std::string_view specularMapPath;


    for (const auto& [key, value] : parser.GetTokensAndValuesMap())
    {
        if (assignValue<std::string_view>("vert_shader", key, value, vertShaderName)) continue;
        if (assignValue<std::string_view>("vert_shader_name", key, value, vertShaderName)) continue;
        if (assignValue<std::string_view>("frag_shader", key, value, vertShaderName)) continue;
        if (assignValue<std::string_view>("frag_shader_name", key, value, vertShaderName)) continue;
        if (assignValue<std::string_view>("diffusepath", key, value, diffuseMapPath)) continue;
        if (assignValue<std::string_view>("specularmap", key, value, specularMapPath)) continue;
        if (assignValue<glm::vec3>("ambient", key, value, material->ambient)) continue;
        if (assignValue<glm::vec3>("diffuse", key, value, material->diffuse)) continue;
        if (assignValue<glm::vec3>("specular", key, value, material->specular)) continue;
        if (assignValue<float>("shininess", key, value, material->shininess)) continue;
    }

    material->shaderHandle = handleShader(vertShaderName, vertShaderPath, Shader::Type::VERTEX, rm, error);
    material->diffuseMap = handleTexMaps(diffuseMapPath, rm, 0);
    material->specularMap = handleTexMaps(specularMapPath, rm, 1);

    return rm.materialPool.ReplaceData(name, std::move(material));
}

void Material::BindShader(ResourceManager &rm) const
{
    if (shaderHandle.gen != 0)
        rm.shaderPool.Get(this->shaderHandle)->Bind();
}
