#pragma once
#include <memory>

#include "scene/Scene.h"
#include "core/Util.h"
#include "core/AssetPool.h"

#include <glm/gtc/matrix_transform.hpp>

FWD_DECLARE_ALL;
class Camera;

namespace Scene
{
    class Scene_Lighting : public Scene::Scene
    {
    public:
        Scene_Lighting();
        ~Scene_Lighting() override;

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
        std::unique_ptr<Camera> camera_;

        Handle h_lightShader;
        Handle h_cubeShader;

        std::unique_ptr<VertexArray> vaObj_;
        std::unique_ptr<VertexBuffer> vbCube_;
        glm::mat4 modelObj_{};

        std::unique_ptr<VertexArray> vaLight_;
        glm::mat4 modelLight_{};


        glm::vec3 red{1.0f, 0.0f, 0.0f};
        glm::vec3 white{1.0f, 1.0f, 1.0f};
        glm::vec3 coral{1.0f, 0.5f, 0.31f};
        glm::vec3 lightColor_{1.0f, 1.0f, 1.0f};
        int cameraModeIndex_;

        Renderer* renderer_;

        float speed_ = 1.0f;
        bool moving_ = true;
        bool debug_ = false;
        bool lockCam_ = false;
        int pow_ = 32;
        float strength_ = 0.5;
    };
}
