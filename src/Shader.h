#pragma once
#include <filesystem>
#include <unordered_map>

#include "glad/gl.h"
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

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform1i(const std::string& name, int v0);
    void SetUniformMat4f(const std::string& name, glm::mat4 matrix);

private:
    int GetUniformLocation(const std::string& name);

    [[nodiscard]] ShaderSourceCode parseShader(const ShaderFilePath& filepaths, int& error);
    [[nodiscard]] GLuint CompileShader(const std::string& source, GLuint type, int& error);
    [[nodiscard]] GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader, int& error);

private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

static unsigned int lastBoundShader = 0;