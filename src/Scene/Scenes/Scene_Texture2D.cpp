#include "Scene_Texture2D.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "../../Util.h"
#include "../../Renderer.h"
#include "../../VertexArray.h"
#include "../../VertexBufferLayout.h"
#include "../../VertexBuffer.h"
#include "../../IndexBuffer.h"
#include "../../Shader.h"
#include "../../Texture.h"


namespace Scene
{
    Scene_Texture2D::Scene_Texture2D() : r(0.0f), inc(0.05f), va(nullptr), ib(nullptr), vb(nullptr), shader(nullptr)
    {
    }

    void Scene_Texture2D::Init()
    {
        const auto& renderer = p_SceneManager_Ref->GetRenderer();

        float vertices[] = {
            -50.0f, -50.0f, 0.0f, 0.0f,
             50.0f, -50.0f, 1.0f, 0.0f,
             50.0f,  50.0f, 1.0f, 1.0f,
            -50.0f,  50.0f, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        vb = new VertexBuffer(vertices, 4 * 4 * sizeof(float));
        vb->Bind();

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va = new VertexArray();
        va->AddBuffer(*vb, layout);

        ib = new IndexBuffer(indices, 6);
        ib->Bind();

        proj = glm::ortho(0.0f, (float)renderer.GetWindowWidth(), 0.0f, (float)renderer.GetWindowHeight(), -1.0f, 1.0f);
        view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/basic.vertGL",
            BASE_PATH / "resources/shaders/basic.fragGL"
        };

        shader = new Shader(paths);
        shader->Bind();
        shader->SetUniform4f("u_Color", 1.0f, 0.5f, 0.5f, 1.0f);

        texture = new Texture(BASE_PATH / "resources/textures/Nian.png");
        texture->Bind(0);
        shader->SetUniform1i("u_Texture", 0);
    }

    Scene_Texture2D::~Scene_Texture2D()
    {
        delete shader;
        delete ib;
        delete va;
    }

    void Scene_Texture2D::Update(float deltaTime)
    {
        if (r > 1.0f) inc = -0.05f;
        else if (r < 0.0f) inc = 0.05f;
        r += inc;
    }

    void Scene_Texture2D::Render()
    {
        if (!p_SceneManager_Ref) return;
        const auto& renderer = p_SceneManager_Ref->GetRenderer();

        shader->Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            glm::mat4 mvp = proj * view * model;
            shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*va, *ib, *shader); // will eventually be va, ib, material
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
            glm::mat4 mvp = proj * view * model;
            shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*va, *ib, *shader); // will eventually be va, ib, material
        }
    }

    void Scene_Texture2D::ImGuiRender()
    {
        ImGui::Begin("Texture2D Test");
        if (ImGui::Button("back"))
            p_SceneManager_Ref->SetScene("Menu");
        ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}
