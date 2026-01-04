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
    Renderer();
    Renderer(int width, int height);

    void Init();
    void Shutdown();
    void Update();

    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;

    [[nodiscard]] bool GetWindowShouldClose() const { return m_WindowShouldClose; }
    [[nodiscard]] int GetWindowWidth() const { return m_Window_Width; }
    [[nodiscard]] int GetWindowHeight() const { return m_Window_Height; }

    void SetTitle(std::string_view title);

    void ImGui_BeginFrame() const;
    void ImGui_EndFrame() const;

private:
    bool m_WindowShouldClose = false;
    GLFWwindow* m_window = nullptr;
    int m_Window_Height = 500;
    int m_Window_Width = 650;
    std::string m_title = "Window Title";
};