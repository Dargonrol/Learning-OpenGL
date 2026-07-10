#pragma once
#include <memory>

#include "Scene/Scene.h"
#include "Extra/Camera.h"
#include "Extra/Objects/Cube.h"


class Model;

namespace Scene
{
    class Scene_ModelLoading : public Scene::Scene
    {
    public:
        Scene_ModelLoading();
        virtual ~Scene_ModelLoading() override;
        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
        void OnResize(int width, int height) override;

    private:
        void HandleInput(float deltaTime);

    private:
        std::unique_ptr<GameObject> object_;
        std::vector<Cube> lights_;
        std::unique_ptr<Camera> camera_;

        float m_camSpeed;
        float m_camSensitivity;
        float m_mouseSensitivity;

        glm::vec3 lightColor_{0.5f, 0.5f, 0.5f};
    };
} // Scene