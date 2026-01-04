#pragma once
#include <functional>
#include <string>
#include <vector>

#include "../Scene.h"

namespace Scene
{
    class Scene_Menu : public Scene::Scene
    {
    public:
        Scene_Menu() = default;
        ~Scene_Menu() override = default;

        void Init() override {}
        void Render() override;
        void Update(float deltaTime) override;
        void ImGuiRender() override;
    };
}
