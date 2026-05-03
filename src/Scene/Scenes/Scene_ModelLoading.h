#pragma once
#include <memory>

#include "../Scene.h"
#include "../../Extra/Camera.h"
#include "../../Extra/Objects/Cube.h"


namespace Scene
{
    class Scene_ModelLoading : public Scene::Scene
    {
        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
        void OnResize(int width, int height) override;

    private:
        std::vector<Object> objects_;
        std::unique_ptr<Camera> camera_;
    };
} // Scene