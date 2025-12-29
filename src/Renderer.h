#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Util.h"
#include "VertexArray.h"
#include "GLFW/glfw3.h"

#ifdef RELEASE
    #define ASSERT(x)
    #define GLCall(x) x;
#else
    #define ASSERT(x) if (!(x)) DEBUG_BREAK();
    #define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Init();
    void Shutdown();
    void Update();

    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;

    bool GetWindowShouldClose() const { return m_WindowShouldClose; }

private:
    bool m_WindowShouldClose = false;
    GLFWwindow* m_window = nullptr;
};