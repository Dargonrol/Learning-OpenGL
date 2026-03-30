#pragma once
#include "../Scene.h"

namespace Scene
{
    class Scene_Material : public Scene
    {
        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
    };
}