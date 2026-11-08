#pragma once
#include <filesystem>
#include <unordered_map>
#include <variant>

#include "glm/fwd.hpp"
#include <vector>

class Shader
{

public:
    static constexpr const char* MATERIAL_BLOCK_NAME = "MaterialBlock";
    static constexpr const char* CAMERA_BLOCK_NAME = "CameraData";

    enum class DataTypeEnum { UNKNOWN = 0, INT, FLOAT, VEC2, VEC3, VEC4, MAT4 };

    using DataType = std::variant<
        int,
        float,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        glm::mat4
    >;

    struct UniformProperty
    {
        DataTypeEnum type;
        size_t size;
        union
        {
            int offset;
            int location;
        };
        std::string name;
    };

    struct ReflectionData
    {
        std::vector<UniformProperty> properties;
        std::vector<UniformProperty> looseUniformsProperties;
        size_t bufferSize = 0;
    };

#ifdef DEBUG_BUILD
    explicit Shader(unsigned int programID, std::string_view name);
#else
    explicit Shader(unsigned int programID);
#endif

    ~Shader();

    void Bind();
    void Unbind();

    [[nodiscard]] unsigned int GetRendererID() const { return m_RendererID; }
    [[nodiscard]] const ReflectionData& GetShaderReflectionData() const { return shaderReflectionData_; }

    void SetUniform1f(const std::string& name, float v0) const;
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;
    void SetUniform1i(const std::string& name, int v0) const;
    void SetUniformVec3(const std::string& name, const glm::vec3& value) const;
    void SetUniformVec4(const std::string& name, const glm::vec4& value) const;
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) const;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

private:
    int GetUniformLocation(const std::string& name) const;
    void SetShaderReflectionData();

private:
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    bool m_GotUnbound = false;
    ReflectionData shaderReflectionData_;

    inline static unsigned int lastBoundShader = 0;

#ifdef DEBUG_BUILD
public:
    std::string DEBUG_shaderName = std::to_string(reinterpret_cast<size_t>(this));
#endif
};