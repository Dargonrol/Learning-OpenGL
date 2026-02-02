#pragma once
#include "../Scene.h"
#include "../../Core/Util.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

FWD_DECLARE_ALL;

namespace Scene
{
    class Scene_BasicPlane : public Scene::Scene
    {
    public:
        Scene_BasicPlane();
        ~Scene_BasicPlane() override;

        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;
        void OnResize(int width, int height) override;

    private:
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;

        glm::mat4 m_model{};
        glm::mat4 m_proj{};
        glm::mat4 m_view{};
    };
}