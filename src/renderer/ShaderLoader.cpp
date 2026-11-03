#include "ShaderLoader.h"

#include <fstream>

#include "core/OpenGL.h"
#include "Shader.h"
#include "core/Log.h"
#include "core/Util.h"

std::optional<Shader> ShaderLoader::FromFiles(const ShaderFilePaths& paths)
{
    Log::Trace("ShaderLoader::FromFiles | Loading shader files...");

    const ShaderSources sources {
        ParseFile(paths.vs),
        ParseFile(paths.tcs),
        ParseFile(paths.tes),
        ParseFile(paths.gs),
        ParseFile(paths.fs),
        ParseFile(paths.cs)
    };

    if (sources.cs.empty())
        return FromSources(sources, paths.vs.stem().string());
    else
        return FromSources(sources, paths.cs.stem().string());
}

std::optional<Shader> ShaderLoader::FromSources(const ShaderSources& sources, std::string_view name)
{
    Log::Trace("ShaderLoader::FromSources | Building shader program '{}'", name);

    std::vector<unsigned int> modules;

    auto addModule = [&modules](const unsigned int id)
    {
        if (id != 0)
            modules.push_back(id);
    };

    if (!sources.vs.empty())
        addModule(CompileShader(sources.vs, GL_VERTEX_SHADER, name));
    if (!sources.tcs.empty())
        addModule(CompileShader(sources.tcs, GL_TESS_CONTROL_SHADER, name));
    if (!sources.tes.empty())
        addModule(CompileShader(sources.tes, GL_TESS_EVALUATION_SHADER, name));
    if (!sources.gs.empty())
        addModule(CompileShader(sources.gs, GL_GEOMETRY_SHADER, name));
    if (!sources.fs.empty())
        addModule(CompileShader(sources.fs, GL_FRAGMENT_SHADER, name));
    if (!sources.cs.empty())
        addModule(CompileShader(sources.cs, GL_COMPUTE_SHADER, name));

    if (modules.empty())
    {
        Log::Error("ShaderLoader::FromSources | No valid shader modules compiled for '{}'", name);
        return std::nullopt;
    }

    const unsigned int programID = LinkProgram(modules, name);

    if (!programID)
    {
        Log::Error("ShaderLoader::FromSources | Failed to create shader '{}'", name);
        return std::nullopt;
    }

    Log::Trace("ShaderLoader::FromSources | Successfully created shader program '{}' (ID: {})", name, programID);

#ifdef DEBUG_BUILD
    return Shader(programID, name);
#else
    return Shader(programID);
#endif
}

std::string ShaderLoader::ParseFile(const std::filesystem::path& path)
{
    if (path.empty())
        return {};

    Log::Trace("ShaderLoader::ParseFile | Reading file: {}", path.string());

    std::error_code ec;
    const auto fileSize = std::filesystem::file_size(path, ec);
    if (ec || fileSize == 0)
    {
        if (ec) Log::Error("ShaderLoader::ParseFile | Invalid file size/path: {}", path.c_str());
        return {};
    }

    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        Log::Error("ShaderLoader::ParseFile | Failed to open file: {}", path.c_str());
        return {};
    }

    std::string buffer(fileSize, '\0');
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    if (static_cast<std::size_t>(file.gcount()) != fileSize)
    {
        Log::Error("ShaderLoader::ParseFile | Failed to read full file: {}", path.c_str());
        return {};
    }

    return buffer;
}

unsigned int ShaderLoader::CompileShader(const std::string& source, const GLenum type, std::string_view name)
{
    if (source.empty())
        return 0;

    Log::Trace("ShaderLoader::CompileShader | {}: Compiling {} shader...", name, GLshaderType_to_String(type));

    const unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    {
        const GLint length = static_cast<GLint>(source.size());
        GLCall(glShaderSource(id, 1, &src, &length));
        GLCall(glCompileShader(id));
    }

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        std::string message(length, '\0');
        GLCall(glGetShaderInfoLog(id, length, &length, message.data()));

        Log::Error("{}: Failed to compile {} shader!\n{}", name, GLshaderType_to_String(type), message);
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int ShaderLoader::LinkProgram(const std::vector<unsigned int>& modules, std::string_view name)
{
    if (modules.empty())
        return 0;

    Log::Trace("ShaderLoader::LinkProgram | {}: Linking shader program with {} module(s)...", name, modules.size());

    const GLuint program = glCreateProgram();

    for (const auto& id: modules)
    {
        GLCall(glAttachShader(program, id));
    }

    GLCall(glLinkProgram(program));
    // GLCall(glValidateProgram(program));

    int linked;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linked));
    if (!linked)
    {
        int length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        std::string message(length, '\0');
        GLCall(glGetProgramInfoLog(program, length, &length, message.data()));

        Log::Error("{}: Failed to link shader!\n{}", name,  message);
        GLCall(glDeleteProgram(program));
    }

    for (const auto& id: modules)
    {
        GLCall(glDeleteShader(id));
    }

    if (!linked)
        return 0;

    return program;
}

std::string ShaderLoader::GLshaderType_to_String(const GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER: return "Vertex";
        case GL_TESS_CONTROL_SHADER: return "Tesselation Control";
        case GL_TESS_EVALUATION_SHADER: return "Tesselation Evaluation";
        case GL_GEOMETRY_SHADER: return "Geometry";
        case GL_FRAGMENT_SHADER: return "Fragment";
        case GL_COMPUTE_SHADER: return "Compute";
        default: return "unknown type";
    }
}
