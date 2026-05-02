#include "Material.h"

#include <fstream>
#include <iostream>

#include "Parser.h"
#include "ResourceManager.h"
#include "Util.h"

struct ShaderContext
{
    std::string_view shaderName;
    std::string_view vertShaderPath, fragShaderPath, tcShaderPath, teShaderPath, geoShaderPath;
};

Handle handleShader(const ShaderContext& shaderContext, ResourceManager& rm, int& error)
{
    if (rm.shaderPool.Exists(shaderContext.shaderName))
        return rm.shaderPool.GetHandle(shaderContext.shaderName);

     std::array<std::filesystem::path, 5> paths = {
         std::filesystem::path(shaderContext.vertShaderPath),
         std::filesystem::path(shaderContext.fragShaderPath),
         std::filesystem::path(shaderContext.tcShaderPath),
         std::filesystem::path(shaderContext.teShaderPath),
         std::filesystem::path(shaderContext.geoShaderPath)
     };

    for (auto& path: paths)
    {
        if (path.empty())
            continue;
        if (path.is_relative())
            path = BASE_PATH / path;
    }

    if (paths[0].empty() || paths[1].empty())
    {
        error = -1;
        std::cerr << "Vertex and Fragment-shader cannot by unspecified!";
        return {0, 0};
    }

    std::unique_ptr<Shader> shader;

    if (!paths[2].empty() && !paths[3].empty() && !paths[4].empty())
    {
        shader = std::make_unique<Shader>(
            paths[0],
            paths[1],
            paths[2],
            paths[3],
            paths[4],
            error
            );
    } else if (!paths[4].empty())
    {
        shader = std::make_unique<Shader>(
            paths[0],
            paths[4],
            paths[1],
            error
            );
    } else
    {
        shader = std::make_unique<Shader>(
            paths[0],
            paths[1],
            error
            );
    }

    if (error)
    {
        std::cerr << "Error parsing material with shader: " << shaderContext.shaderName << std::endl;
        return {0, 0};
    }

    std::string shaderName = std::string(shaderContext.shaderName);
    if (!shaderName.empty())
        shaderName = std::string(shaderContext.shaderName);

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
bool assignValue(const Parser::Value& value, T& target)
{
    if (const T* p = std::get_if<T>(&value))
    {
        target = *p;
        return true;
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
    const auto enumValueMap = parser.getTokensAsEnum(Material::tokenEnumStrMap);

    ShaderContext shader_context;
    std::string_view diffuseMapPath, specularMapPath;


    for (const auto& [key, value] : enumValueMap)
    {
        switch (key)
        {
            case Tokens::SHADER_NAME:
                assignValue<std::string_view>(value, shader_context.shaderName);
                break;

            case Tokens::VERT_SHADER_PATH:
                assignValue<std::string_view>(value, shader_context.vertShaderPath);
                break;
            case Tokens::FRAG_SHADER_PATH:
                assignValue<std::string_view>(value, shader_context.fragShaderPath);
                break;
            case Tokens::TC_SHADER_PATH:
                assignValue<std::string_view>(value, shader_context.tcShaderPath);
                break;
            case Tokens::TE_SHADER_PATH:
                assignValue<std::string_view>(value, shader_context.teShaderPath);
                break;
            case Tokens::GEO_SHADER_PATH:
                assignValue<std::string_view>(value, shader_context.geoShaderPath);
                break;

            case Tokens::DIFF_MAP_PATH:
                assignValue<std::string_view>(value, diffuseMapPath);
                break;
            case Tokens::SPEC_MAP_PATH:
                assignValue<std::string_view>(value, specularMapPath);
                break;

            case Tokens::AMBIENT_VALUE:
                assignValue<glm::vec3>(value, material->ambient);
                break;
            case Tokens::DIFFUSE_VALUE:
                assignValue<glm::vec3>(value, material->diffuse);
                break;
            case Tokens::SPECULAR_VALUE:
                assignValue<glm::vec3>(value, material->specular);
                break;
            case Tokens::SHININESS_VALUE:
                assignValue<float>(value, material->shininess);
                break;
        }
    }

    material->shaderHandle = handleShader(shader_context, rm, error);
    material->diffuseMap = handleTexMaps(diffuseMapPath, rm, 0);
    material->specularMap = handleTexMaps(specularMapPath, rm, 1);

    return rm.materialPool.ReplaceData(name, std::move(material));
}

void Material::BindShader(ResourceManager &rm) const
{
    if (shaderHandle.gen != 0)
        rm.shaderPool.Get(this->shaderHandle)->Bind();
}
