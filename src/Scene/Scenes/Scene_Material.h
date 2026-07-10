#pragma once
#include <memory>

#include "Core/AssetPool.h"
#include "Scene/Scene.h"
#include "Extra/Camera.h"
#include "Extra/LightObject.h"
#include "Extra/GameObject.h"


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
        void OnResize(int width, int height) override;

    private:
        std::unique_ptr<GameObject> cube_;
        std::unique_ptr<LightObject> light_;
        Handle debugMaterial = {0, 0};
        std::unique_ptr<Camera> camera_;

        glm::vec3 lightColor_{0.5f, 0.5f, 0.5f};

    };
}
