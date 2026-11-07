#include "Shader.h"

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "core/Util.h"
#include "core/OpenGL.h"

#ifdef DEBUG_BUILD
Shader::Shader(const unsigned int programID, const std::string_view name)
{
    m_RendererID = programID;
    DEBUG_shaderName = std::string(name);
    SetShaderReflectionData();
}
#else
Shader::Shader(const unsigned int programID)
{
    m_RendererID = programID;
    SetShaderReflectionData();
}
#endif

Shader::~Shader()
{
    if (m_RendererID)
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

void Shader::SetUniform1i(const std::string &name, const int v0) const { GLCall(glUniform1i(GetUniformLocation(name), v0)); }

void Shader::SetUniformVec3(const std::string &name, const glm::vec3 &value) const { GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z)); }

void Shader::SetUniformVec4(const std::string &name, const glm::vec4 &value) const{ GLCall(glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3])); }

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4& matrix) const { GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0])); }

void Shader::SetUniform1f(const std::string &name, const float v0) const { GLCall(glUniform1f(GetUniformLocation(name), v0)); }

void Shader::SetUniform4f(const std::string &name, const float v0, const float v1, const float v2, const float v3) const { GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3)); }

int Shader::GetUniformLocation(const std::string &name) const
{
    if (m_UniformLocationCache.contains(name))
        return m_UniformLocationCache[name];

    int location;
    GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        Log::Warn("Uniform '{}' does not exist!", name);
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::SetShaderReflectionData()
{
#ifdef DEBUG_BUILD
    Log::Debug("{}: Setting Shader reflection data", DEBUG_shaderName);
#endif

    Bind();

    const unsigned int blockIndex = glGetUniformBlockIndex(m_RendererID, MATERIAL_BLOCK_NAME);
    if (blockIndex != GL_INVALID_INDEX)
    {
        int blockSize = 0;
        GLCall(glGetActiveUniformBlockiv(m_RendererID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize));
        Log::Trace("Block index: {}, Block size / buffer size: {}", blockIndex, blockSize);
        shaderReflectionData_.bufferSize = static_cast<size_t>(blockSize);
        // shaderReflectionData_.blockIndex = blockIndex;
    }
    else
    {
        shaderReflectionData_.bufferSize = 0;
        Log::Trace("Buffer size: 0");
    }

    int numUniforms = 0;
    GLCall(glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &numUniforms));
    Log::Trace("Number of Uniforms: {}", numUniforms);

    if (numUniforms <= 0)
        return;

    std::vector<GLuint> uniformIndices(numUniforms);
    for (int i = 0; i < numUniforms; ++i)
    {
        uniformIndices[i] = static_cast<GLuint>(i);
    }

    std::vector<GLint> offsets(numUniforms);

    GLCall(glGetActiveUniformsiv(m_RendererID, numUniforms, uniformIndices.data(), GL_UNIFORM_OFFSET, offsets.data()));

    for (int i = 0; i < numUniforms; ++i)
    {
        char nameBuffer[256];
        GLsizei actualLength = 0;
        GLint uniformSize = 0;
        GLenum uniformType = 0;
        GLCall(glGetActiveUniform(m_RendererID, uniformIndices[i], static_cast<GLsizei>(sizeof(nameBuffer)), &actualLength, &uniformSize, &uniformType, nameBuffer));

        UniformProperty prop;
        prop.name = std::string(nameBuffer);
        prop.size = static_cast<size_t>(uniformSize);
        switch (uniformType)
        {
        case GL_INT:
            Log::Trace("Found {} Uniform | Type: GL_INT/INT | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::INT;
            break;
        case GL_SAMPLER_2D:
            Log::Trace("Found {} Uniform | Type: GL_SAMPLER_2D/INT | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::INT;
            break;
        case GL_FLOAT:
            Log::Trace("Found {} Uniform | Type: GL_FLOAT/FLOAT | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::FLOAT;
            break;
        case GL_FLOAT_VEC2:
            Log::Trace("Found {} Uniform | Type: GL_FLOAT_VEC2/FLOAT | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::VEC2;
            break;
        case GL_FLOAT_VEC3:
            Log::Trace("Found {} Uniform | Type: GL_FLOAT_VEC3/VEC3 | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::VEC3;
            break;
        case GL_FLOAT_VEC4:
            Log::Trace("Found {} Uniform | Type: GL_FLOAT_VEC4/VEC4 | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::VEC4;
            break;
        case GL_FLOAT_MAT4:
            Log::Trace("Found {} Uniform | Type: GL_FLOAT_MAT4/MAT4 | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::MAT4;
            break;
        default:
            Log::Trace("Found {} Uniform | Type: ?/UNKNOWN | Name: {} | Size: {}", offsets[i] >= 0 ? "UBO" : "loose", nameBuffer, uniformSize);
            prop.type = DataTypeEnum::UNKNOWN;
            break;
        }

        if (const int offset = offsets[i]; offset >= 0)
        {
            prop.offset = offset;
            shaderReflectionData_.properties.push_back(prop);
        } else
        {
            prop.location = GetUniformLocation(prop.name);
            shaderReflectionData_.looseUniformsProperties.push_back(prop);
        }
    }
}

Shader & Shader::operator=(Shader &&other) noexcept
{
    if (this != &other)
    {
        if (m_RendererID != 0)
            glDeleteProgram(m_RendererID);

        m_RendererID = other.m_RendererID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        m_GotUnbound = other.m_GotUnbound;
        shaderReflectionData_ = std::move(other.shaderReflectionData_);

        other.m_RendererID = 0;
    }
    return *this;
}

Shader::Shader(Shader &&other) noexcept : m_RendererID(other.m_RendererID), m_UniformLocationCache(std::move(other.m_UniformLocationCache)), m_GotUnbound(other.m_GotUnbound), shaderReflectionData_((std::move(other.shaderReflectionData_))) { other.m_RendererID = 0; }