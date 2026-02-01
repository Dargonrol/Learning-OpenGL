#include "Shader.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"

Shader::Shader(const ShaderFilePath &filepaths, int& error)
{
    ShaderSourceCode shaderSource = parseShader(filepaths, error);
    if (shaderSource.fragmentShader.empty() || shaderSource.vertexShader.empty())
        return;
    m_RendererID = CreateShader(shaderSource.vertexShader, shaderSource.fragmentShader, error);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    if (!m_GotUnbound && lastBoundShader == m_RendererID)
        return;
    GLCall(glUseProgram(m_RendererID));
    lastBoundShader = m_RendererID;
}

void Shader::Unbind()
{
    GLCall(glUseProgram(0));
    m_GotUnbound = true;
}

void Shader::SetUniform1i(const std::string &name, int v0)
{
    GLCall(glUniform1i(GetUniformLocation(name), v0));
}

void Shader::SetUniformMat4f(const std::string &name, glm::mat4 matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

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

ShaderSourceCode Shader::parseShader(const ShaderFilePath& filepaths, int& error)
{
    std::string line;
    std::stringstream ss[2];

    std::ifstream stream(filepaths.vertexShader);
    if (!stream.is_open())
    {
        std::cerr << "Failed to open vertex shader: " << std::filesystem::absolute(filepaths.vertexShader) << std::endl;
        if (error == 0)
            error = -1;
        return {};
    }

    while (getline(stream, line))
    {
        if (line.find("#shader") == std::string::npos)
        {
            ss[0] << line << '\n';
        }
    }

    std::ifstream stream2(filepaths.fragmentShader);
    if (!stream2.is_open())
    {
        std::cerr << "Failed to open fragment shader: " << std::filesystem::absolute(filepaths.fragmentShader) << std::endl;
        if (error == 0)
            error = -1;
        return {};
    }

    while (getline(stream2, line))
        if (line.find("#shader") == std::string::npos)
        {
            ss[1] << line << '\n';
        }

    return {ss[0].str(), ss[1].str() };
}

GLuint Shader::CompileShader(const std::string& source, GLuint type, int& error)
{
    std::string typeString;
    switch (type)
    {
        case GL_VERTEX_SHADER:
            typeString = "vertex";
            break;

        case GL_FRAGMENT_SHADER:
            typeString = "fragment";
            break;

        default:
            typeString = "unknown";
    }

    std::cout << "Compiling " << typeString << " shader" << std::endl;

    GLuint id = glCreateShader(type);
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

        std::cout << "failed to compile " << typeString.c_str() << " shader!" << std::endl;
        std::cout << message << std::endl;
        if (error == 0)
            error = -1;
        glDeleteShader(id);
        return 0;
    }

    std::cout << "Successfully compiled " << typeString << " shader" << std::endl;

    return id;
}

GLuint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, int& error)
{
    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(vertexShader, GL_VERTEX_SHADER, error);
    GLuint fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER, error);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}