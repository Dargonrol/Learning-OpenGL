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

    enum class Type
    {
        VERTEX,
        TESS_CONTROL,
        TESS_EVAL,
        GEOMETRY,
        FRAGMENT,
        COMPUTE
    };

    struct ShaderContext
    {
        std::filesystem::path vs;
        std::filesystem::path tcs;
        std::filesystem::path tes;
        std::filesystem::path gs;
        std::filesystem::path fs;
        std::filesystem::path cs;
    };

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

    Shader(
        const ShaderContext& shaderContext,
        int& error
        );
    Shader(
        const std::filesystem::path &vs,
        const std::filesystem::path &tcs,
        const std::filesystem::path &tes,
        const std::filesystem::path &gs,
        const std::filesystem::path &fs,
        int& error
        );
    Shader(
        const std::filesystem::path& vs,
        const std::filesystem::path &gs,
        const std::filesystem::path& fs,
        int& error
        );
    Shader(
        const std::filesystem::path& vs,
        const std::filesystem::path& fs,
        int& error
        );
    Shader(
        const std::filesystem::path& cs,
        int& error
        );
    Shader(
        const std::filesystem::path& path,
        Type type,
        int& error
        );
    Shader(
        const std::string& vs,
        const std::string& fs,
        int& error
        );
    Shader(
        const char* vs,
        const char* fs,
        int& error
        );
    ~Shader();

    void Bind();
    void Unbind();

    [[nodiscard]] unsigned int GetRendererID() const { return m_RendererID; }
    [[nodiscard]] const ReflectionData& GetShaderReflectionData() const { return shaderReflectionData_; }

    void SetUniform1f(const std::string& name, float v0);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform1i(const std::string& name, int v0);
    void SetUniformVec3(const std::string& name, const glm::vec3& value);
    void SetUniformVec4(const std::string& name, const glm::vec4& value);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

private:
    struct ShaderFilePaths
    {
        std::filesystem::path vertexShader;
        std::filesystem::path tesselationControlShader;
        std::filesystem::path tesselationEvaluateShader;
        std::filesystem::path geometryShader;
        std::filesystem::path fragmentShader;
        std::filesystem::path computeShader;
    };
    struct ShaderSourceCode
    {
        std::string vertexShader;
        std::string tesselationControlShader;
        std::string tesselationEvaluateShader;
        std::string geometryShader;
        std::string fragmentShader;
        std::string computeShader;
    };

    Shader(
        const std::filesystem::path &vs,
        const std::filesystem::path &tcs,
        const std::filesystem::path &tes,
        const std::filesystem::path &gs,
        const std::filesystem::path &fs,
        const std::filesystem::path &cs,
        int& error
        );

    int GetUniformLocation(const std::string& name);
    void SetShaderReflectionData();

    [[nodiscard]] static ShaderSourceCode parseShaders(const ShaderFilePaths& filepaths, int& error);
    [[nodiscard]] static unsigned int CompileShader(const std::string& source, unsigned int type, int& error);
    [[nodiscard]] static unsigned int CreateShaders(const ShaderSourceCode& source, int& error);

private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    bool m_GotUnbound = false;
    bool valid_ = false;
    ReflectionData shaderReflectionData_;

    inline static unsigned int lastBoundShader = 0;
};