#pragma once
#include "../Scene.h"
#include <glm/gtc/matrix_transform.hpp>

#include "../../IndexBuffer.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VertexArray.h"

namespace Scene
{
    class Scene_Simple3D : public Scene::Scene
    {
    public:
        Scene_Simple3D() = default;

        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;

    private:
        glm::mat4 m_model[10];
        glm::mat4 m_view;
        glm::mat4 m_proj;

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

        float rotationAngles[10] = {0};

        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;

        std::unique_ptr<Texture> m_tex1;
        std::unique_ptr<Texture> m_tex2;
    };
}
