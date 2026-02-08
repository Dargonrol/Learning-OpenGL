#pragma once
#include <filesystem>
#include <unordered_map>

#include "glm/fwd.hpp"

struct ShaderSourceCode
{
    std::string vertexShader;
    std::string fragmentShader;
};

struct ShaderFilePath
{
    std::filesystem::path vertexShader;
    std::filesystem::path fragmentShader;
};

class Shader
{
public:
    Shader(const ShaderFilePath& filepaths, int& error);
    ~Shader();

    void Bind();
    void Unbind();

    [[nodiscard]] unsigned int GetRendererID() const { return m_RendererID; }

    void SetUniform1f(const std::string& name, float v0);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform1i(const std::string& name, int v0);
    void SetUniformVec3(const std::string& name, const glm::vec3& value);
    void SetUniformVec4(const std::string& name, const glm::vec4& value);
    void SetUniformMat4f(const std::string& name, glm::mat4 matrix);

private:
    int GetUniformLocation(const std::string& name);

    [[nodiscard]] ShaderSourceCode parseShader(const ShaderFilePath& filepaths, int& error);
    [[nodiscard]] unsigned int CompileShader(const std::string& source, unsigned int type, int& error);
    [[nodiscard]] unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, int& error);

private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
    bool m_GotUnbound = false;
};

static unsigned int lastBoundShader = 0;