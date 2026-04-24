#include "Material.h"

#include <fstream>
#include <iostream>

#include "Parser.h"
#include "ResourceManager.h"
#include "Util.h"

Handle handleShader(const std::string_view shaderName, const std::filesystem::path& shaderPath, ResourceManager& rm, int& error)
{
    if (rm.shaderPool.Exists(shaderName))
        return rm.shaderPool.GetHandle(shaderName);

    auto shader = std::make_unique<Shader>(BASE_PATH / shaderPath, shaderName.data(), error);
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

Handle Material::parseMaterial(std::string_view name, const std::filesystem::path &path, ResourceManager& rm, int& error, bool replace)
{
    if (!replace)
        if (rm.materialPool.Exists(name))
            return rm.materialPool.GetHandle(name);

    auto material = std::make_unique<Material>();

    Parser parser(path);
    parser.Parse();

    std::string shaderName;
    std::string shaderPath;
    std::string diffuseMapPath;
    std::string specularMapPath;

    for (const auto& [key, value] : parser.GetTokensAndValuesMap())
    {
        if (key == "shadername")
        {
            if (const std::string_view* str = std::get_if<std::string_view>(&value))
                shaderName = std::string(*str);
            continue;
        }

        if (key == "shaderpath")
        {
            if (const auto* str = std::get_if<std::string_view>(&value))
                shaderPath = std::string(*str);
            continue;
        }
    }


    shaderName = parseStringFromKeyWord(line, "shadername", '"');
    shaderPath = parseStringFromKeyWord(line, "shaderpath", '"');
    diffuseMapPath = parseStringFromKeyWord(line, "diffusemap", '"');
    specularMapPath = parseStringFromKeyWord(line, "specularmap", '"');
    material->ambient = parseVec3FromKeyWord(line, "ambient");
    material->diffuse = parseVec3FromKeyWord(line, "diffuse");
    material->specular = parseVec3FromKeyWord(line, "specular");
    material->shininess = parseFloatFromKeyWord(line, "shininess");

    material->shaderHandle = handleShader(shaderName, shaderPath, rm, error);
    material->diffuseMap = handleTexMaps(diffuseMapPath, rm, 0);
    material->specularMap = handleTexMaps(specularMapPath, rm, 1);

    return rm.materialPool.ReplaceData(name, std::move(material));
}

void Material::BindShader(ResourceManager &rm) const
{
    if (shaderHandle.gen != 0)
        rm.shaderPool.Get(this->shaderHandle)->Bind();
}

