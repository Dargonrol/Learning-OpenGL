#include "Material.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "core/ResourceManager.h"
#include "core/Util.h"

void Material::Init(const Handle shaderHandle, ResourceManager& rm)
{
    const Shader* shader = rm.shaderPool.Get(shaderHandle);
    if (!shader)
    {
        std::cerr << "[ERROR] Material::Init(): Shader handle not valid!" << std::endl;
        return;
    }

    shaderLayout_ = shader->GetShaderReflectionData();
    uniformBufferData_.resize(shaderLayout_.bufferSize);

    shaderHandle_ = shaderHandle;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
void Material::Bind(ResourceManager& rm)
{
    Shader* shader = rm.shaderPool.Get(shaderHandle_);
    if (!shader)
        return;

    shader->Bind();

    for (size_t i = 0; i < textureSlotsBuffer_.size(); ++i)
    {
        std::string_view name = textureSlotsBuffer_[i].samplerName;
        const Handle handle = textureSlotsBuffer_[i].textureHandle;
        const Texture* tex = rm.texturePool.Get(handle);
        if (!tex)
            continue;
        tex->Bind(i);

        shader->SetUniform1i(name.data(), i);
    }

    for (const auto& [key, value]: looseUniformBuffer_)
    {
        const auto& propertyVector = shaderLayout_.looseUniformsProperties;
        // TODO: potential performance issue? store uniform loc in map instead of name?
        const auto& iter = std::find_if(propertyVector.begin(), propertyVector.end(), [key](const Shader::UniformProperty& prop){ return prop.name == key; });
        if (iter == propertyVector.end())
            continue;
        unsigned int location = iter->location;
        std::visit(overloaded {
            [location](const int arg) { GLCall(glUniform1i(location, arg)); },
            [location](const float arg) { GLCall(glUniform1f(location, arg)); },
            [location](const glm::vec2& arg) { GLCall(glUniform2f(location, arg[0],arg[1])); },
            [location](const glm::vec3& arg) { GLCall(glUniform3f(location, arg[0],arg[1],arg[2])); },
            [location](const glm::vec4& arg) { GLCall(glUniform4f(location, arg[0],arg[1],arg[2],arg[3])); },
            [location](const glm::mat4& arg) { GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &arg[0][0])); }
        }, value);
    }
}

void Material::SetValue(const std::string_view name, const Shader::DataType& value)
{
    // look if name is in looseUniformBuffer. if yes set value
    {
        const auto iter = looseUniformBuffer_.find(name);
        if (iter != looseUniformBuffer_.end())
        {
            iter->second = value;
            return;
        }
    }

    // since name is not in looseUniformBuffer look if there is an entry in the looseUniformProp vector for name. If yes set value.
    const auto& looseUniformProperties = shaderLayout_.looseUniformsProperties;
    const auto iter = std::find_if(looseUniformProperties.begin(), looseUniformProperties.end(), [name](const Shader::UniformProperty& property)
    {
        return property.name == name;
    });

    if (iter != looseUniformProperties.end())
    {
        looseUniformBuffer_[std::string(name)] = value;
        return;
    }

    // name is not a looseUniform so it has to be part of the buffer or is not defined.
    if (!uniformBufferData_.empty())
    {
        for (const auto& prop: shaderLayout_.properties)
        {
            if (prop.name == name)
            {
                uint8_t* ptr = &uniformBufferData_[prop.offset];

                std::visit([ptr](const auto& val)
                {
                    using T = std::decay_t<decltype(val)>;
                    std::memcpy(ptr, &val, sizeof(T));
                }, value);

                return;
            }
        }
    } // TODO: uniform prop data doesnt seem to get set properly...
}

void Material::SetTexture(const std::string_view samplerName, const Handle handle)
{
    for (auto& slot : textureSlotsBuffer_)
    {
        if (slot.samplerName == samplerName)
        {
            slot.textureHandle = handle;
            return;
        }
    }
    textureSlotsBuffer_.push_back({std::string{samplerName}, handle});
}

[[nodiscard]] Shader::DataTypeEnum GetTypeEnum(const Shader::DataType& data)
{
    return std::visit(overloaded {
                    [](const int& arg)       { return Shader::DataTypeEnum::INT; },
                    [](const float& arg)     { return Shader::DataTypeEnum::FLOAT; },
                    [](const glm::vec2& arg) { return Shader::DataTypeEnum::VEC2; },
                    [](const glm::vec3& arg) { return Shader::DataTypeEnum::VEC3; },
                    [](const glm::vec4& arg) { return Shader::DataTypeEnum::VEC4; },
                    [](const glm::mat4& arg) { return Shader::DataTypeEnum::MAT4; }
    }, data);
}

template<typename T>
Shader::DataType ReadBufferValue(const uint8_t* ptr)
{
    T returnValue{};
    std::memcpy(&returnValue, ptr, sizeof(T));
    return returnValue;
}

Shader::DataType Material::GetValue(const std::string_view name, Shader::DataTypeEnum* type)
{
    if (!looseUniformBuffer_.empty())
    {
        const auto iter = looseUniformBuffer_.find(name);
        if (iter != looseUniformBuffer_.end())
        {
            const auto returnValue = iter->second;
            if (type)
                *type = GetTypeEnum(returnValue);
            return returnValue;
        }
    }

    if (!uniformBufferData_.empty())
    {
        for (const auto& prop: shaderLayout_.properties)
        {
            if (prop.name == name)
            {
                if (type)
                    *type = prop.type;

                const uint8_t* ptr = &uniformBufferData_[prop.offset];

                switch (prop.type)
                {
                case Shader::DataTypeEnum::INT:     return ReadBufferValue<int>(ptr);
                case Shader::DataTypeEnum::FLOAT:   return ReadBufferValue<float>(ptr);
                case Shader::DataTypeEnum::VEC2:    return ReadBufferValue<glm::vec2>(ptr);
                case Shader::DataTypeEnum::VEC3:    return ReadBufferValue<glm::vec3>(ptr);
                case Shader::DataTypeEnum::VEC4:    return ReadBufferValue<glm::vec4>(ptr);
                case Shader::DataTypeEnum::MAT4:    return ReadBufferValue<glm::mat4>(ptr);
                default: break;
                }
            }
        }
    }

    if (type)
        *type = Shader::DataTypeEnum::UNKNOWN;
    return {};
}

Handle Material::GetShaderHandle() const
{
    return shaderHandle_;
}

[[nodiscard]] inline std::filesystem::path SanitizePath(const std::filesystem::path& sourcePath)
{
    std::filesystem::path sanitizedPath = sourcePath;

    if (sanitizedPath.is_relative())
        sanitizedPath = BASE_PATH / sanitizedPath;

    sanitizedPath = sanitizedPath.lexically_normal();

    sanitizedPath.make_preferred();

    std::error_code e;
    auto canonicalPath = std::filesystem::weakly_canonical(sanitizedPath, e);
    if (!e)
        return canonicalPath;
    return sanitizedPath;
}

using json = nlohmann::json;

Handle Material::ParseMaterial(const std::filesystem::path& materialPath, ResourceManager& rm,
    int& error, bool replace)
{
    std::ifstream file(materialPath);
    if (!file.is_open())
    {
         std::cerr << "[ERROR] Material::ParseMaterial: Could not open file " << materialPath << std::endl;
        error = -1;
        return {0, 0};
    }

    json data;
    try
    {
        file >> data;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "[ERROR] Material::ParseMaterial: JSON error in " << materialPath << ": " << e.what() << std::endl;
        error = -1;
        return {0 ,0};
    }

    std::string materialName;
    if (!data.contains("material_name") || !data["material_name"].is_string())
    {
        error = -1;
        std::cerr << "[ERROR] Material::ParseMaterial: \"material_name\" cannot be empty! in " << materialPath << std::endl;
        return {0 ,0};
    }
    materialName = data["material_name"];

    Handle shaderHandle{};
    std::string shaderName;
    if (data.contains("shader_name") && data["shader_name"].is_string())
        shaderName = data["shader_name"].get<std::string>();
    else
    {
        std::cerr << "[ERROR] Material::ParseMaterial: shader_name cannot be empty! in " << materialPath << std::endl;
        error = -1;
        return {0, 0};
    }

    if (!rm.shaderPool.Exists(shaderName))
    {
        if (!data.contains("shader_paths"))
        {
            error = -1;
            std::cerr << "[ERROR] Material::ParseMaterial: Shader: " << shaderName << " did not exist and no shader_paths were provided." << std::endl;
            return {0, 0};
        }

        const auto& paths = data["shader_paths"];

        Shader::ShaderContext sc;
        if (paths.contains("vertex_shader") && paths["vertex_shader"].is_string())
            sc.vs = SanitizePath(paths["vertex_shader"].get<std::string>());
        if (paths.contains("compute_shader") && paths["compute_shader"].is_string())
            sc.cs = SanitizePath(paths["compute_shader"].get<std::string>());
        if (paths.contains("fragment_shader") && paths["fragment_shader"].is_string())
            sc.fs = SanitizePath(paths["fragment_shader"].get<std::string>());
        if (paths.contains("geometry_shader") && paths["geometry_shader"].is_string())
            sc.gs = SanitizePath(paths["geometry_shader"].get<std::string>());
        if (paths.contains("tess_control_shader") && paths["tess_control_shader"].is_string())
            sc.tcs = SanitizePath(paths["tess_control_shader"].get<std::string>());
        if (paths.contains("tess_eval_shader") && paths["tess_eval_shader"].is_string())
            sc.tes = SanitizePath(paths["tess_eval_shader"].get<std::string>());

        std::cout << sc.vs.c_str() << std::endl << std::flush;
        std::cout << sc.fs.c_str() << std::endl << std::flush;

        Shader shader{sc, error};
        if (error != 0)
        {
            std::cerr << "[ERROR] Material::ParseMaterial: failed shader creation with shader: " << shaderName << std::endl;
            return {0, 0};
        }

        shaderHandle = rm.shaderPool.Register(shaderName, std::move(shader));
    } else
        shaderHandle = rm.shaderPool.GetHandle(shaderName);

    Material material;
    material.Init(shaderHandle, rm);

    if (data.contains("uniforms") && data["uniforms"].is_object())
    {
        for (const auto& [name, val] : data["uniforms"].items())
        {
            if (val.is_number_float())
                material.SetValue(name, val.get<float>());
            else if (val.is_number_integer())
                material.SetValue(name, val.get<int>());
            else if (val.is_array())
            {
                size_t arraySize = val.size();

                if (arraySize == 2)
                {
                    glm::vec2 vec{val[0].get<float>(), val[1].get<float>()};
                    material.SetValue(name, vec);
                }
                else if (arraySize == 3)
                {
                    glm::vec3 vec{val[0].get<float>(), val[1].get<float>(), val[2].get<float>()};
                    material.SetValue(name, vec);
                }
                else if (arraySize == 4)
                {
                    glm::vec4 vec{val[0].get<float>(), val[1].get<float>(), val[2].get<float>(),val[3].get<float>()};
                    material.SetValue(name, vec);
                }
                else if (arraySize == 16)
                {
                    glm::mat4 mat{1.0f};
                    for (int i = 0; i < 16; ++i)
                    {
                        mat[i / 4][i % 4] = val[i].get<float>();
                    }
                    material.SetValue(name, mat);
                }
            }
        }
    }

    if (data.contains("textures") && data["textures"].is_object())
    {
        for (const auto& [samplerName, path] : data["textures"].items())
        {
            if (path.is_string())
            {
                std::filesystem::path texturePath = SanitizePath(path.get<std::string>());
                std::string textureKey = texturePath.string();

                Handle handle = rm.texturePool.GetHandle(textureKey);
                if (!handle)
                {
                    Texture texture{texturePath};
                    handle = rm.texturePool.Register(textureKey, std::move(texture));
                }
                material.SetTexture(samplerName, handle);
            }
        }
    }

    if (replace)
        return rm.materialPool.ReplaceData(materialName, material);
    else
        return rm.materialPool.Register(materialName, material);


}
