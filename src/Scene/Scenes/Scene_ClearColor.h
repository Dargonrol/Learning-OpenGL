#pragma once
#include "../Scene.h"

namespace Scene
{
    class Scene_ClearColor : public Scene::Scene
    {
    public:
        Scene_ClearColor();
        ~Scene_ClearColor() override = default;

        int Init() override { return 0; }
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;

        void OnEnter() override {};

    private:
        float m_ClearColor[4];
    };
}
