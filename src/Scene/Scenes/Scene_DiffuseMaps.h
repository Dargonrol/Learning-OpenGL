#pragma once
#include <memory>

#include "../Scene.h"
#include "../../Extra/Camera.h"
#include "../../Extra/Objects/Cube.h"


namespace Scene
{
    class Scene_DiffuseMaps : public Scene::Scene
    {
        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
        void OnResize(int width, int height) override;

    private:
        std::unique_ptr<Cube> cube_;
        std::unique_ptr<Cube> light_;
        Handle debugMaterial = {0, 0};
        std::unique_ptr<Camera> camera_;

        glm::vec3 lightColor_{0.5f, 0.5f, 0.5f};
    };
} // Scene