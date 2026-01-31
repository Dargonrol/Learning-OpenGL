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

    private:
        glm::mat4 m_model{1.0f};
        glm::mat4 m_view{1.0f};
        glm::mat4 m_proj{};

        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<VertexArray> m_va;
        std::unique_ptr<VertexBuffer> m_vb;
        std::unique_ptr<IndexBuffer> m_ib;

        std::unique_ptr<Texture> m_tex1;
        std::unique_ptr<Texture> m_tex2;
    };
}
