#pragma once
#include <memory>

#include "../Scene.h"
#include "../../Core/Util.h"
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

    private:
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vbLight;
        std::unique_ptr<VertexBuffer> m_vbCube;
        std::unique_ptr<IndexBuffer> m_ib;
        std::unique_ptr<Camera> m_camera;

        glm::mat4 m_modelCube{};
        glm::mat4 m_modelLight{};
        glm::mat4 m_proj{};
    };
}
