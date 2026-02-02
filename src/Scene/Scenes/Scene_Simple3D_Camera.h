#pragma once
#include "../Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "../../Core/Util.h"

FWD_DECLARE_ALL;
class Texture;
class Camera;

namespace Scene
{
    class Scene_Simple3D_Camera : public Scene::Scene
    {
    public:
        Scene_Simple3D_Camera();
        ~Scene_Simple3D_Camera() override;

        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
        void OnResize(int width, int height) override;

    private:
        void ResetCamera();
        void HandleInput(float deltaTime);

    private:
        glm::mat4 m_model[10]{1.0f};
        //glm::mat4 m_proj{1.0f};

        glm::vec3 cubePositions[10]{
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        float rotationAngles[10] = {0.0f};

        std::unique_ptr<Camera> m_camera;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;

        std::unique_ptr<Texture> m_tex1;
        std::unique_ptr<Texture> m_tex2;

        float m_camSpeed;
        float m_camSensitivity;
        float m_mouseSensitivity;

        // ImGui
        int m_cameraModeIndex = 0;
        int m_prevTabState;

        // mouse
        double m_mouseX, m_mouseY;
        double m_mouseLastX, m_mouseLastY;
    };
}
