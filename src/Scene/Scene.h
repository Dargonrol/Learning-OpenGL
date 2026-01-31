#pragma once

namespace Scene
{
    class SceneManager;

    class Scene
    {
    public:
        friend class SceneManager;

        virtual ~Scene() = default;

        virtual void Init() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Render() = 0;
        virtual void ImGuiRender() = 0;
        virtual void OnEnter() {};
        virtual void OnLeave();

    protected:
        SceneManager* p_SceneManager_Ref;
    };
}
