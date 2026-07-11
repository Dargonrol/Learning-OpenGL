#pragma once
#include <memory>
#include <string_view>
#include <string>

namespace Scene
{
    class SceneManager;
}

class Renderer;
class ResourceManager;
struct GLFWwindow;

namespace Core
{
    class Application
    {
    public:
        Application(int width, int height, std::string_view title);
        ~Application();

        void Run();

        void CaptureMouse() const;
        void ReleaseMouse() const;

        void SetTitle(std::string_view sv) const;
        void QueueCloseWindow();

        [[nodiscard]] int GetWindowWidth() const;
        [[nodiscard]] int GetWindowHeight() const;

    private:
        void InitWindow(int width, int height, std::string_view title);
        void InitImGui() const;

        void Update(float deltaTime);
        static void FrameBufferCallback(GLFWwindow* window, int width, int height);

    private:
        int width_;
        int height_;
        std::string title_;

        GLFWwindow* window_ = nullptr;

        std::unique_ptr<ResourceManager> resourceManager_;
        std::unique_ptr<Scene::SceneManager> sceneManager_;
        std::unique_ptr<Renderer> renderer_;

        float lastFrameTime_ = 0.0f;
        bool windowShouldClose_ = false;
    };
}
