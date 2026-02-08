#pragma once

#include <string>
#include "Util.h"

class VertexArray;
class IndexBuffer;
class Shader;
class GLFWwindow;

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    Renderer();
    Renderer(int width, int height);
    ~Renderer();

    void Init();
    void Shutdown() const;
    void Update();

    void Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const;
    void Draw(const VertexArray& va, Shader& shader, unsigned int count) const;
    void Clear() const;

    void CaptureMouse() const;
    void ReleaseMouse() const;

    [[nodiscard]] bool GetWindowShouldClose() const { return m_WindowShouldClose; }
    [[nodiscard]] int GetWindowWidth();
    [[nodiscard]] int GetWindowHeight();
    [[nodiscard]] GLFWwindow& GetWindow() const { return *m_window; }
    [[nodiscard]] std::string_view GetDefaultTitle() const;
    [[nodiscard]] int GetDefaultWindowWidth() const;
    [[nodiscard]] int GetDefaultWindowHeight() const;

    void SetTitle(std::string_view title);
    void SetDefaultTitle(std::string_view title);
    void SetWindowShouldClose() { m_WindowShouldClose = true; }
    void SetDefaultWindowWidth(int v);
    void SetDefaultWindowHeight(int v);
    void SetWindowWidth(int value);
    void SetWindowHeight(int value);

    void ImGui_BeginFrame() const;
    void ImGui_EndFrame() const;

private:
    bool m_WindowShouldClose = false;
    GLFWwindow* m_window = nullptr;
    int m_Window_Height = 500;
    int m_Window_Width = 650;
    int defaultWindowWidth = 500;
    int defaultWindowHeight = 500;
    std::string m_Title = "Window Title";
    std::string m_DefaultTitle;
};