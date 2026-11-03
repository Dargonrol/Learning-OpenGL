#pragma once
#include <filesystem>
#include <optional>
#include <vector>


class Shader;

typedef unsigned int GLenum;

struct ShaderFilePaths
{
    std::filesystem::path vs;
    std::filesystem::path tcs;
    std::filesystem::path tes;
    std::filesystem::path gs;
    std::filesystem::path fs;
    std::filesystem::path cs;
};

struct ShaderSources
{
    std::string vs;
    std::string tcs;
    std::string tes;
    std::string gs;
    std::string fs;
    std::string cs;
};

class ShaderLoader
{
public:
    [[nodiscard]] static std::optional<Shader> FromFiles(const ShaderFilePaths& paths);
    [[nodiscard]] static std::optional<Shader> FromSources(const ShaderSources& sources, std::string_view name = "unknown shader");

private:
    static std::string ParseFile(const std::filesystem::path& path);
    static unsigned int CompileShader(const std::string& source, GLenum type, std::string_view name);
    static unsigned int LinkProgram(const std::vector<unsigned int>& modules, std::string_view name);
    static std::string GLshaderType_to_String(GLenum type);
};
