#include "Renderer.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#undef GLAD_GL_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stb_image.h>
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "../Extra/Object.h"
#include "glm/gtc/type_ptr.hpp"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << ") in " << function << " at " << file << ":" << line << "\n";
        return false;
    }
    return true;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

Renderer::Renderer() {};

Renderer::Renderer(int width, int height, ResourceManager& resource_manager) : m_Window_Height(height), m_Window_Width(width), rm_(&resource_manager){}

void Renderer::Init()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_Window_Width, m_Window_Height, m_Title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendEquation(GL_FUNC_ADD));

    m_window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330"); // <- hier fehlt es bei dir
    ImGui::StyleColorsDark();

    glfwSetWindowTitle(m_window, m_Title.c_str());

    stbi_set_flip_vertically_on_load(true);

    // set up basic shader and material
    const char* vertex =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 uMVP;\n"
        "void main() { gl_Position = uMVP * vec4(aPos, 1.0); }\n";

    const char* frag =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main(){FragColor = vec4(1.0, 0.0, 0.0, 1.0);}\n";
    int error = 0;
    auto shader = std::make_unique<Shader>(vertex, frag, error);
    if (error)
    {
        std::cerr << "Failed Debug Shader creation!" << std::endl;
        return;
    }
    auto material = std::make_unique<Material>();
    material->shaderHandle = rm_->shaderPool.Register("debug", std::move(shader));
    debugShaderHandle = material->shaderHandle;
    material->ambient = {1.0f, 0.0f, 0.0f};
    material->diffuse = {0.0f, 0.0f, 0.0f};
    material->shininess = 0.0f;
    material->specular = {0.0f, 0.0f, 0.0f};
    debugMaterialHandle = rm_->materialPool.Register("debug", std::move(material));
    std::cout << "Registering debug Material and Shader \n";
}

void Renderer::Update()
{
    if (glfwWindowShouldClose(m_window))
        m_WindowShouldClose = true;
    glfwPollEvents();
}

void Renderer::Shutdown() const
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, Shader &shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray &va, Shader &shader, unsigned int count) const
{
    shader.Bind();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
}

void Renderer::Draw(Object &obj) const
{
    obj.BindAll(*rm_);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, obj.GetVerticesCount()));
}

void Renderer::Draw(Object &obj, Handle materialHandle) const
{
    obj.BindAll(*rm_);
    rm_->materialPool.Get(materialHandle)->BindShader(*rm_);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, obj.GetVerticesCount()));
}

void Renderer::WireDraw(Object &obj, glm::mat4& MVP) const
{
    obj.BindAll(*rm_);
    if (rm_->shaderPool.Exists(debugShaderHandle))
    {
        rm_->shaderPool.Get(debugShaderHandle)->Bind();
        rm_->shaderPool.Get(debugShaderHandle)->SetUniformMat4f("uMVP", MVP);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, obj.GetVerticesCount()));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (rm_->shaderPool.Exists(debugShaderHandle))
        rm_->shaderPool.Get(debugShaderHandle)->Unbind();
}

void Renderer::Clear() const
{
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::CaptureMouse() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
}

void Renderer::ReleaseMouse() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

int Renderer::GetWindowWidth()
{
    glfwGetFramebufferSize(m_window, &m_Window_Width, &m_Window_Height);
    return m_Window_Width;
}

int Renderer::GetWindowHeight()
{
    glfwGetFramebufferSize(m_window, &m_Window_Width, &m_Window_Height);
    return m_Window_Height;
}

std::string_view Renderer::GetDefaultTitle() const
{
    return m_DefaultTitle;
}

int Renderer::GetDefaultWindowWidth() const
{
    return defaultWindowWidth;
}

int Renderer::GetDefaultWindowHeight() const
{
    return defaultWindowHeight;
}

void Renderer::SetDefaultWindowWidth(int v)
{
    defaultWindowWidth = v;
}

void Renderer::SetDefaultWindowHeight(int v)
{
    defaultWindowHeight = v;
}

void Renderer::SetWindowWidth(int value)
{
    m_Window_Width = value;
}

void Renderer::SetWindowHeight(int value)
{
    m_Window_Height = value;
}

void Renderer::SetTitle(std::string_view title)
{
    m_Title = std::string(title);
    if (m_window)
        glfwSetWindowTitle(m_window, m_Title.c_str());
}

void Renderer::SetDefaultTitle(std::string_view title)
{
    m_DefaultTitle = std::string(title);
}

void Renderer::ImGui_BeginFrame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::ImGui_EndFrame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

Renderer::~Renderer() = default;
