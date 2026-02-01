#pragma once

#include "../Scene.h"

namespace Scene
{
    class Scene_Menu : public Scene::Scene
    {
    public:
        Scene_Menu();
        ~Scene_Menu() override;

        int Init() override { return 0; }
        void Render() override;
        void Update(float deltaTime) override;
        void ImGuiRender() override;
        void OnEnter() override;

        static int s_prevEscapeState; // keep previous state
    };
}
