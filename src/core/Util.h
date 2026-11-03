#pragma once
#include <iostream>

#include "Log.h"

#if defined(__clang__) || defined(__GNUC__)
    #define DEBUG_BREAK() __builtin_trap()
#elif defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
#else
    #include <signal.h>
    #define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifndef SOURCE_DIR
    #define SOURCE_DIR ""
#endif
#define BASE_PATH std::filesystem::path(SOURCE_DIR)

#ifdef RELEASE
    #define ASSERT(x)
    #define GLCall(x) x;
#else
#include "core/OpenGL.h"

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << ") in " << function << " at " << file << ":" << line << "\n";
        return false;
    }
    return true;
}
    #define ASSERT(x) if (!(x)) DEBUG_BREAK();
    #define GLCall(x) {GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))}
#endif

#define FWD_DECLARE_ALL class IndexBuffer;\
    class VertexBuffer;\
    class VertexArray;\
    class Shader

inline void debug_info()
{
#ifdef DEBUG_BUILD
    Log::Debug("running debug build");
    if constexpr (sizeof(void*) == 8) {
        Log::Debug("Runtime: 64-bit process");
    } else if constexpr (sizeof(void*) == 4) {
        Log::Debug("Runtime: 32-bit process");
    } else {
        Log::Error("Unknown Pointer size");
    }
#endif
}