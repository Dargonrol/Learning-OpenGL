#pragma once
#include <memory>

#include "scene/Scene.h"
#include "scene/Camera.h"


class LightObject;
class GameObject;

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
        std::unique_ptr<GameObject> cube_;
        std::unique_ptr<LightObject> light_;
        std::unique_ptr<Camera> camera_;

        glm::vec3 lightColor_{0.5f, 0.5f, 0.5f};
    };
} // Scene