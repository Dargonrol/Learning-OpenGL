#pragma once
#include <string>

class Renderer;

namespace Scene
{
    class SceneManager;

    class Scene
    {
    public:
        friend class SceneManager;

        virtual ~Scene() = default;

        virtual int Init() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Render() = 0;
        virtual void ImGuiRender() = 0;
        virtual void OnEnter() {};
        virtual void OnLeave();
        virtual void OnResize(int width, int height) {};

        bool initialized = false;
        std::string name;

    protected:
        SceneManager* sceneManager_;
        Renderer* renderer_;
    };
}
