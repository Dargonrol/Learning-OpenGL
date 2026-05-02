#include "Shader.h"
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Util.h"
#include "../OpenGL.h"

Shader::Shader(const std::filesystem::path &vs, const std::filesystem::path &tcs, const std::filesystem::path &tes, const std::filesystem::path &gs, const std::filesystem::path &fs, const std::filesystem::path &cs, int &error)
{
    error = 0;
    const ShaderFilePaths paths = {
        vs,
        tcs,
        tes,
        gs,
        fs,
        cs
    };

    const ShaderSourceCode source = parseShaders(paths, error);
    if (error)
    {
        valid_ = false;
        return;
    }
    m_RendererID = CreateShaders(source, error);
    if (!error)
        valid_ = true;
}

Shader::Shader(const ShaderContext &shaderContext, int &error) : Shader(shaderContext.vs, shaderContext.tcs, shaderContext.tes, shaderContext.gs, shaderContext.fs, shaderContext.cs, error) {}

Shader::Shader(const std::filesystem::path &vs, const std::filesystem::path &tcs, const std::filesystem::path &tes, const std::filesystem::path &gs, const std::filesystem::path &fs, int &error) : Shader(vs, tcs, tes, gs, fs, {}, error) {}

Shader::Shader(const std::filesystem::path& vs, const std::filesystem::path &gs, const std::filesystem::path& fs, int &error) : Shader(vs, {}, {}, gs, fs, error){}

Shader::Shader(const std::filesystem::path& vs, const std::filesystem::path& fs, int &error) : Shader(vs, {}, fs, error) {}

Shader::Shader(const std::filesystem::path& cs, int &error) : Shader({}, {}, {}, {}, {}, cs, error) {}

Shader::Shader(const std::string &vs, const std::string &fs, int &error)
{
    error = 0;
    ShaderSourceCode shaderSource = {
        vs,
        "",
        "",
        "",
        fs,
        ""
    };
    m_RendererID = CreateShaders(shaderSource, error);
    if (!error)
        valid_ = true;
}

Shader::Shader(const char *vs, const char *fs, int &error) : Shader(std::string(vs), std::string(fs), error) {}

Shader::~Shader()
{
    if (valid_)
    {
        GLCall(glDeleteProgram(m_RendererID));
    }
}

void Shader::Bind()
{
    if (!m_GotUnbound && lastBoundShader == m_RendererID)
        return;
    GLCall(glUseProgram(m_RendererID));
    lastBoundShader = m_RendererID;
    m_GotUnbound = false;
}

void Shader::Unbind()
{
    GLCall(glUseProgram(0));
    m_GotUnbound = true;
}

void Shader::SetUniform1i(const std::string &name, const int v0) { GLCall(glUniform1i(GetUniformLocation(name), v0)); }

void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &value) { GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z)); }

void Shader::SetUniformVec4(const std::string &name, const glm::vec4 &value) { GLCall(glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3])); }

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4& matrix) { GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0])); }

void Shader::SetUniform1f(const std::string &name, const float v0) { GLCall(glUniform1f(GetUniformLocation(name), v0)); }

void Shader::SetUniform4f(const std::string &name, const float v0, const float v1, const float v2, const float v3) { GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); }

int Shader::GetUniformLocation(const std::string &name)
{
    if (m_UniformLocationCache.contains(name))
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "[WARNING] Uniform '" << name << "' does not exist!\n";
    }
    m_UniformLocationCache[name] = location;
    return location;
}

GLuint Shader::CompileShader(const std::string& source, unsigned int type, int& error)
{
    std::string typeString;
    switch (type)
    {
        case GL_VERTEX_SHADER:
            typeString = "vertex";
            break;

        case GL_TESS_CONTROL_SHADER:
            typeString = "tessellation control";
            break;

        case GL_TESS_EVALUATION_SHADER:
            typeString = "tessellation evaluation";
            break;

        case GL_GEOMETRY_SHADER:
            typeString = "geometry";
            break;

        case GL_FRAGMENT_SHADER:
            typeString = "fragment";
            break;

        case GL_COMPUTE_SHADER:
            typeString = "compute";
            break;

        default:
            typeString = "unknown";
    }

    const GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::string message;
        message.resize(length);
        glGetShaderInfoLog(id, length, &length, message.data());

        std::cerr << "failed to compile " << typeString.c_str() << " shader!" << std::endl;
        std::cerr << message << std::endl;
        if (error == 0)
            error = -1;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShaders(const Shader::ShaderSourceCode& source, int& error)
{
    std::cout << "Compiling Shaders...\n" << std::flush;
    const GLuint program = glCreateProgram();
    GLuint vs = 0;
    GLuint tcs = 0;
    GLuint tes = 0;
    GLuint gs = 0;
    GLuint fs = 0;
    GLuint cs = 0;
    if (!source.vertexShader.empty())
    {
        vs = CompileShader(source.vertexShader, GL_VERTEX_SHADER, error);
        glAttachShader(program, vs);
    }
    if (!source.tesselationControlShader.empty())
    {
        tcs = CompileShader(source.tesselationControlShader, GL_TESS_CONTROL_SHADER, error);
        glAttachShader(program, tcs);
    }
    if (!source.tesselationEvaluateShader.empty())
    {
        tes = CompileShader(source.tesselationEvaluateShader, GL_TESS_EVALUATION_SHADER, error);
        glAttachShader(program, tes);
    }
    if (!source.geometryShader.empty())
    {
        gs = CompileShader(source.geometryShader, GL_GEOMETRY_SHADER, error);
        glAttachShader(program, gs);
    }
    if (!source.fragmentShader.empty())
    {
        fs = CompileShader(source.fragmentShader, GL_FRAGMENT_SHADER, error);
        glAttachShader(program, fs);
    }
    if (!source.computeShader.empty())
    {
        cs = CompileShader(source.computeShader, GL_COMPUTE_SHADER, error);
        glAttachShader(program, cs);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    int linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string message(length, '\0');
        glGetProgramInfoLog(program, length, &length, message.data());
        std::cerr << "Failed to link shader program:\n" << message << std::endl;
        error = -2;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(gs);
    glDeleteShader(fs);
    glDeleteShader(cs);

    return program;
}

std::string parseFile(const std::filesystem::path& path, const std::string& shaderType, int& error)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Failed to open " << shaderType << " shader: " << std::filesystem::absolute(path) << std::endl;
        error = -1;
        return {};
    }
    std::streamsize size = file.tellg();
    file.seekg(0);

    std::string buffer(size, '\0');
    file.read(buffer.data(), size);

    if (!file)
    {
        std::cerr << "Failed to read " << shaderType << " shader: " << std::filesystem::absolute(path) << std::endl;
        error = -2;
        return {};
    }

    return buffer;
}

Shader::ShaderSourceCode Shader::parseShaders(const ShaderFilePaths& filepaths, int& error)
{
    std::string shaders[6];
    if (!filepaths.vertexShader.empty()) shaders[0] = parseFile(filepaths.vertexShader, "vertex", error); if (error) return {};
    if (!filepaths.tesselationControlShader.empty()) shaders[1] = parseFile(filepaths.tesselationControlShader, "tessellation control", error); if (error) return {};
    if (!filepaths.tesselationEvaluateShader.empty()) shaders[2] = parseFile(filepaths.tesselationEvaluateShader, "tessellation evaluation", error); if (error) return {};
    if (!filepaths.geometryShader.empty()) shaders[3] = parseFile(filepaths.geometryShader, "geometry", error); if (error) return {};
    if (!filepaths.fragmentShader.empty()) shaders[4] = parseFile(filepaths.fragmentShader, "fragment", error); if (error) return {};
    if (!filepaths.computeShader.empty()) shaders[5] = parseFile(filepaths.computeShader, "compute", error); if (error) return {};

    return {
        shaders[0],
        shaders[1],
        shaders[2],
        shaders[3],
        shaders[4],
        shaders[5]
    };
}

Shader & Shader::operator=(Shader &&other) noexcept
{
    if (this != &other)
    {
        if (m_RendererID != 0 && valid_)
            glDeleteProgram(m_RendererID);

        m_RendererID = other.m_RendererID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        m_GotUnbound = other.m_GotUnbound;

        other.m_RendererID = 0;
    }
    return *this;
}

Shader::Shader(Shader &&other) noexcept : m_RendererID(other.m_RendererID), m_UniformLocationCache(std::move(other.m_UniformLocationCache)), m_GotUnbound(other.m_GotUnbound) { other.m_RendererID = 0; }