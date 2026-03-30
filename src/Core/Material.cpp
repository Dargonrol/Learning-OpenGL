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

    return rm.shaderPool.Register(shaderName, std::move(shader));
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

Handle Material::parseMaterial(std::string_view name, const std::filesystem::path &path, ResourceManager& rm, int& error)
{
    if (rm.materialPool.Exists(name))
        return rm.materialPool.GetHandle(name);

    Material material;
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
    while (getline(stream, line))
    {
        std::string_view sv(line);

        if (sv.find("shadername") != std::string::npos)
        {
            size_t start = sv.find('"');
            size_t end = sv.rfind('"');

            if (start != std::string::npos && end != std::string::npos && end > start)
                shaderName = line.substr(start + 1, end - start - 1);
            continue;
        }

        if (sv.find("shaderpath") != std::string::npos)
        {
            size_t start = sv.find('"');
            size_t end = sv.rfind('"');

            if (start != std::string::npos && end != std::string::npos && end > start)
                shaderPath = line.substr(start + 1, end - start - 1);
            continue;
        }

        if (sv.find("ambient") != std::string::npos)
        {
                material.ambient = parseVec3(line);
                continue;
        }

        if (sv.find("diffuse") != std::string::npos)
        {
            material.diffuse = parseVec3(line);
            continue;
        }

        if (sv.find("specular") != std::string::npos)
        {
            material.specular = parseVec3(line);
            continue;
        }

        if (sv.find("shininess") != std::string::npos)
        {
            size_t start = sv.find('=');

            if (start != std::string::npos)
            {
                std::string part = line.substr(start + 1);
                part.erase(0, part.find_first_not_of(" \t"));
                part.erase(part.find_last_not_of(" \t") + 1);

                if (!part.empty() && part.back() == 'f')
                    part.pop_back();

                material.shininess = std::stof(part);
            }
            continue;
        }
    }

    material.shaderHandle = handleShader(shaderName, shaderPath, rm, error);

    return rm.materialPool.Register(name, std::make_unique<Material>(material));
}

void Material::BindShader(ResourceManager &rm) const
{
    if (shaderHandle.gen != 0)
        rm.shaderPool.Get(this->shaderHandle)->Bind();
}

