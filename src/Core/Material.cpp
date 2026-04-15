#include "Material.h"

#include <fstream>
#include <iostream>

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

std::string parseStringFromKeyWord(const std::string& line,const std::string_view keyWord, const char delimiter)
{
    if (line.find(keyWord) != std::string::npos)
    {
        const size_t start = line.find(delimiter);
        const size_t end = line.rfind(delimiter);

        if (start != std::string::npos && end != std::string::npos && end > start)
            return line.substr(start + 1, end - start - 1);
    }
    return "";
}

float parseFloat(const std::string& line)
{
    size_t start = line.find('=');

    if (start != std::string::npos)
    {
        std::string part = line.substr(start + 1);
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);

        if (!part.empty() && part.back() == 'f')
            part.pop_back();

        return std::stof(part);
    }
}

float parseFloatFromKeyWord(const std::string& line, const std::string_view keyWord)
{
    if (line.find(keyWord))
        return parseFloat(line);
    return 0.0f;
}

glm::vec3 parseVec3(const std::string& line)
{
    glm::vec3 result{0.0f};

    size_t start = line.find('{');
    size_t end = line.rfind('}');

    if (start != std::string::npos && end != std::string::npos && end > start)
    {
        std::string part = line.substr(start + 1, end - start - 1);


        std::stringstream ss(part);
        std::string item;
        int i = 0;

        while (std::getline(ss, item, ',') && i < 3)
        {
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);

            if (!item.empty() && item.back() == 'f')
                item.pop_back();

            result[i] = std::stof(item);
            ++i;
        }
    }
    return result;
}

glm::vec3 parseVec3FromKeyWord(const std::string& line, const std::string_view keyWord)
{
    if (line.find(keyWord) != std::string::npos)
    {
        return parseVec3(line);
    }
    return {};
}

Handle Material::parseMaterial(std::string_view name, const std::filesystem::path &path, ResourceManager& rm, int& error, bool replace)
{
    if (!replace)
        if (rm.materialPool.Exists(name))
            return rm.materialPool.GetHandle(name);

    auto material = std::make_unique<Material>();
    std::string line;
    std::stringstream ss;

    std::ifstream stream(path);
    if (!stream.is_open())
    {
        std::cerr << "Failed to open material-file: " << std::filesystem::absolute(path) << std::endl;
        if (error == 0)
            error = -1;
        return {};
    }


    std::string shaderName;
    std::string shaderPath;
    std::string diffuseMapPath;
    std::string specularMapPath;
    while (getline(stream, line))
    {
        shaderName = parseStringFromKeyWord(line, "shadername", '"');
        shaderPath = parseStringFromKeyWord(line, "shaderpath", '"');
        diffuseMapPath = parseStringFromKeyWord(line, "diffusemap", '"');
        specularMapPath = parseStringFromKeyWord(line, "specularmap", '"');
        material->ambient = parseVec3FromKeyWord(line, "ambient");
        material->diffuse = parseVec3FromKeyWord(line, "diffuse");
        material->specular = parseVec3FromKeyWord(line, "specular");
        material->shininess = parseFloatFromKeyWord(line, "shininess");
    }

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

