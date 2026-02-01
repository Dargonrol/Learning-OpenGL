#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "../Scene.h"
#include "../SceneManager.h"
#include "../../Texture.h"

namespace Scene
{
    class Scene_Texture2D : public Scene
    {
    public:
        Scene_Texture2D();
        ~Scene_Texture2D() override = default;

        int Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;
        void OnLeave() override;

    private:
        float r = 0.0f;
        float inc = 0.05f;

        std::unique_ptr<VertexArray> va;
        std::unique_ptr<IndexBuffer> ib;
        std::unique_ptr<VertexBuffer> vb;
        std::unique_ptr<Texture> texture;
        std::unique_ptr<Shader> shader;

        glm::vec3 translationA{200, 200, 0};
        glm::vec3 translationB{400, 200, 0};
        glm::mat4 proj{};
        glm::mat4 view{};
    };
}
