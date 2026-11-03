#pragma once
#include "core/AssetPool.h"
#include <filesystem>

#include "Shader.h"

// fwd declares
class Shader;
class ResourceManager;
// fwd declares

struct StringHash
{
    using is_transparent = void;

    size_t operator()(const std::string_view sv) const noexcept
    {
        return std::hash<std::string_view>{}(sv);
    }
    size_t operator()(const std::string& str) const noexcept
    {
        return std::hash<std::string>{}(str);
    }
    size_t operator()(const char* str) const noexcept
    {
        return std::hash<std::string_view>{}(str);
    }
};

class Material
{
public:
    struct TextureBinding
    {
        std::string samplerName;
        Handle textureHandle;
    };

    Material() = default;
    ~Material() = default;

    void Init(Handle shaderHandle, ResourceManager& rm);

    void Bind(ResourceManager& rm);

    void SetValue(std::string_view name, const Shader::DataType& value);
    void SetTexture(std::string_view samplerName, Handle handle);
    [[nodiscard]] Shader::DataType GetValue(std::string_view name, Shader::DataTypeEnum* type = nullptr);
    [[nodiscard]] Handle GetShaderHandle() const;

    static Handle ParseMaterial(const std::filesystem::path& materialPath, ResourceManager& rm, int& error, bool replace = false);

private:
    std::vector<uint8_t> uniformBufferData_;
    std::vector<TextureBinding> textureSlotsBuffer_;
    std::unordered_map<std::string, Shader::DataType, StringHash, std::equal_to<>> looseUniformBuffer_;
    Shader::ReflectionData shaderLayout_;
    Handle shaderHandle_;
};
