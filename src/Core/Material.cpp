#include "Material.h"

#include <fstream>
#include <iostream>

#include "Parser.h"
#include "ResourceManager.h"
#include "Util.h"

struct ShaderContext
{
    std::string vertShaderPath, fragShaderPath, tcShaderPath, teShaderPath, geoShaderPath;
};

inline std::filesystem::path makeSVPathAbsolute(const std::string_view& sv)
{
    if (sv.empty())
        return "";
    std::filesystem::path path(sv);
    if (path.is_relative())
        path = BASE_PATH / path;
    return path;
}

Handle handleShader(const ShaderContext& shaderContext, const std::string_view& shaderName, ResourceManager& rm, int& error, const std::string_view& matName)
{
    if (shaderName.empty())
    {
        error = -1;
        std::cerr << "Error creating Material " << matName << ": Shader_name cannot be empty!" << std::endl;
        return {0, 0};
    }

    if (rm.shaderPool.Exists(shaderName))
        return rm.shaderPool.GetHandle(shaderName);

    Shader::ShaderContext sc = {
        makeSVPathAbsolute(shaderContext.vertShaderPath),
        makeSVPathAbsolute(shaderContext.tcShaderPath),
        makeSVPathAbsolute(shaderContext.teShaderPath),
        makeSVPathAbsolute(shaderContext.geoShaderPath),
        makeSVPathAbsolute(shaderContext.fragShaderPath),
        {}
    };

    auto shader = std::make_unique<Shader>(sc, error);

    if (error)
    {
        std::cerr << "Error parsing material with shader: " << shaderName << std::endl;
        return {0, 0};
    }

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
    std::string shaderName;
    std::string diffuseMapPath, specularMapPath;


    for (const auto& [key, value] : enumValueMap)
    {
        switch (key)
        {
            case Tokens::SHADER_NAME:
                assignValue<std::string>(value, shaderName);
                break;

            case Tokens::VERT_SHADER_PATH:
                assignValue<std::string>(value, shader_context.vertShaderPath);
                break;
            case Tokens::FRAG_SHADER_PATH:
                assignValue<std::string>(value, shader_context.fragShaderPath);
                break;
            case Tokens::TC_SHADER_PATH:
                assignValue<std::string>(value, shader_context.tcShaderPath);
                break;
            case Tokens::TE_SHADER_PATH:
                assignValue<std::string>(value, shader_context.teShaderPath);
                break;
            case Tokens::GEO_SHADER_PATH:
                assignValue<std::string>(value, shader_context.geoShaderPath);
                break;

            case Tokens::DIFF_MAP_PATH:
                assignValue<std::string>(value, diffuseMapPath);
                break;
            case Tokens::SPEC_MAP_PATH:
                assignValue<std::string>(value, specularMapPath);
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

    material->shaderHandle = handleShader(shader_context, shaderName, rm, error, name);
    material->diffuseMap = handleTexMaps(diffuseMapPath, rm, 0);
    material->specularMap = handleTexMaps(specularMapPath, rm, 1);

    return rm.materialPool.ReplaceData(name, std::move(material));
}

void Material::BindShader(ResourceManager &rm) const
{
    if (shaderHandle.gen != 0)
        rm.shaderPool.Get(this->shaderHandle)->Bind();
}
