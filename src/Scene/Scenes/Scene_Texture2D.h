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
        ~Scene_Texture2D() override;

        void Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void ImGuiRender() override;
        void OnEnter() override;

    private:
        float r = 0.0f;
        float inc = 0.05f;

        VertexArray* va;
        IndexBuffer* ib;
        VertexBuffer* vb;
        Texture* texture{};
        Shader* shader;
        glm::vec3 translationA{200, 200, 0};
        glm::vec3 translationB{400, 200, 0};
        glm::mat4 proj{};
        glm::mat4 view{};
    };
}
