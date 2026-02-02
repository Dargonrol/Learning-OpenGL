#pragma once

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
    #define ASSERT(x) if (!(x)) DEBUG_BREAK();
    #define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif

#define FWD_DECLARE_ALL class IndexBuffer;\
    class VertexBuffer;\
    class VertexArray;\
    class Shader