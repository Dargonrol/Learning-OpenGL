#include "Application.h"

#define GLFW_INCLUDE_NONE

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#undef GLAD_GL_IMPLEMENTATION
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/ResourceManager.h"
#include "core/SceneManager.h"
#include "renderer/Renderer.h"
#include "sandbox/Scene_Menu.h"
#include "sandbox/01_ClearColor/Scene_ClearColor.h"
#include "sandbox/02_BasicPlane/Scene_BasicPlane.h"
#include "sandbox/03_Texture2D/Scene_Texture2D.h"
#include "sandbox/04_Simple3D/Scene_Simple3D.h"
#include "sandbox/05_Simple3D_Camera/Scene_Simple3D_Camera.h"
#include "sandbox/06_Lighting/Scene_Lighting.h"
#include "sandbox/07_Material/Scene_Material.h"
#include "sandbox/08_DiffuseMaps/Scene_DiffuseMaps.h"
#include "sandbox/09_ModelLoading/Scene_ModelLoading.h"

namespace Core
{
    static void error_callback(int error, const char* description)
    {
        Log::Error("glfw: {}", description);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
    }

    Application::Application(const int width, const int height, const std::string_view title) : width_(width), height_(height)
    {
        Log::Init();
#ifndef RELEASE
        debug_info();
#endif
        using namespace std::literals;

        Log::Trace("Application: Initializing Window");
        InitWindow(width, height, title);
        Log::Trace("Application: Initializing ImGui");
        InitImGui();
        Log::Trace("Application: Creating ResourceManager");
        resourceManager_ = std::make_unique<ResourceManager>();
        Log::Trace("Application: Creating and Initializing Renderer");
        renderer_ = std::make_unique<Renderer>(*resourceManager_, window_, *this);
        renderer_->Init();
        Log::Trace("Application: Creating sceneManager");
        sceneManager_ = std::make_unique<Scene::SceneManager>(*renderer_, *resourceManager_);

        sceneManager_->RegisterScene<Scene::Scene_Menu>("Menu"sv);
        sceneManager_->RegisterScene<Scene::Scene_ClearColor>("01 Clear Color"sv);
        sceneManager_->RegisterScene<Scene::Scene_BasicPlane>("02 Basic Plane");
        sceneManager_->RegisterScene<Scene::Scene_Texture2D>("03 Texture2D"sv);
        sceneManager_->RegisterScene<Scene::Scene_Simple3D>("04 Simple3D"sv);
        sceneManager_->RegisterScene<Scene::Scene_Simple3D_Camera>("05 Simple3D Camera"sv);
        sceneManager_->RegisterScene<Scene::Scene_Lighting>("06 Lighting"sv);
        sceneManager_->RegisterScene<Scene::Scene_Material>("07 Material"sv);
        sceneManager_->RegisterScene<Scene::Scene_DiffuseMaps>("08 Diffuse Maps"sv);
        sceneManager_->RegisterScene<Scene::Scene_ModelLoading>("09 Model Loading"sv);
        sceneManager_->SetScene<Scene::Scene_Menu>();

        Log::Debug("#######################################");
        Log::Debug("        Finished initializing!         ");
        Log::Debug("#######################################");
    }

    Application::~Application()
    {
        Log::Trace("Application: Shutting down...");
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window_);
        glfwTerminate();
        Log::Shutdown();
        exit(EXIT_SUCCESS);
    }

    void Application::Run()
    {
        while (!windowShouldClose_)
        {
            const auto currentFrame = static_cast<float>(glfwGetTime());
            const float deltaTime = currentFrame - lastFrameTime_;
            lastFrameTime_ = currentFrame;

            Update(deltaTime);

            sceneManager_->Render();
        }
    }

    void Application::CaptureMouse() const
    {
        Log::Trace("Application: set mouse capturing = true");
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    }

    void Application::ReleaseMouse() const
    {
        Log::Trace("Application: set mouse capturing = false");
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void Application::SetTitle(const std::string_view sv) const
    {
        Log::Trace("Application: setting title: {}", sv);
        if (window_)
            glfwSetWindowTitle(window_, sv.data());
    }

    void Application::QueueCloseWindow()
    {
        Log::Trace("Application: queue window termination");
        windowShouldClose_ = true;
    }

    int Application::GetWindowWidth() const
    {
        return width_;
    }

    int Application::GetWindowHeight() const
    {
        return height_;
    }

    ResourceManager* Application::GetResourceManager() const
    {
        return &*resourceManager_;
    }

    void Application::InitWindow(const int width, const int height, const std::string_view title)
    {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit())
            exit(EXIT_FAILURE);

        Log::Trace("Application: setting window hints");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Log::Trace("Application: creating window");
        GLFWwindow* window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        if (!window)
        {
            Log::Critical("Application: Window creation failed!");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, key_callback);

        Log::Trace("Application: load GL via glad");
        glfwMakeContextCurrent(window);
        gladLoadGL();
        glfwSwapInterval(1);

        Log::Debug("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FrameBufferCallback);

        window_ = window;
    }

    void Application::InitImGui() const
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGui::StyleColorsDark();
    }

    void Application::Update(const float deltaTime)
    {
        glfwGetFramebufferSize(window_, &width_, &height_);
        sceneManager_->Update(deltaTime);

        if (glfwWindowShouldClose(window_))
            windowShouldClose_ = true;
        glfwPollEvents();
    }

    void Application::FrameBufferCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app)
            app->sceneManager_->OnResize(width, height);
    }
}
