#include "Scene_Texture2D.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "Scene_Menu.h"
#include "../SceneManager.h"
#include "../../Core/Util.h"
#include "../../Core/Renderer.h"
#include "../../Core/VertexArray.h"
#include "../../Core/VertexBufferLayout.h"
#include "../../Core/VertexBuffer.h"
#include "../../Core/IndexBuffer.h"
#include "../../Core/Shader.h"
#include "../../Core/Texture.h"


namespace Scene
{
    using std::literals::operator ""sv;

    Scene_Texture2D::Scene_Texture2D() : va(nullptr), ib(nullptr), vb(nullptr), texture(nullptr),
                                         shader(nullptr) {}

    Scene_Texture2D::~Scene_Texture2D() = default;

    int Scene_Texture2D::Init()
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

        vb = std::make_unique<VertexBuffer>(vertices, 4 * 4 * sizeof(float));
        vb->Bind();

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va = std::make_unique<VertexArray>();
        va->AddBuffer(*vb, layout);

        ib = std::make_unique<IndexBuffer>(indices, 6);
        ib->Bind();

        proj = glm::ortho(0.0f, (float)renderer.GetWindowWidth(), 0.0f, (float)renderer.GetWindowHeight(), -1.0f, 1.0f);
        view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/Basic/basic_VERT.shader",
            BASE_PATH / "resources/shaders/Basic/basic_FRAG.shader"
        };

        int error = 0;
        shader = std::make_unique<Shader>(paths, error);
        if (error)
            return error;
        shader->Bind();
        shader->SetUniform4f("u_Color", 1.0f, 0.5f, 0.5f, 1.0f);

        texture = std::make_unique<Texture>(BASE_PATH / "resources/textures/Nian.png");
        texture->Bind(0);
        shader->SetUniform1i("u_Texture", 0);

        return error;
    }

    void Scene_Texture2D::Update(float deltaTime)
    {
        if (r > 1.0f) inc = -0.05f;
        else if (r < 0.0f) inc = 0.05f;
        r += inc;

        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (keyState == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene<Scene_Menu>();
        }
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

    void Scene_Texture2D::OnEnter()
    {
        p_SceneManager_Ref->GetRenderer().SetTitle("Texture 2D test"sv);
        shader->Bind();
        texture->Bind(0);
    }

    void Scene_Texture2D::OnLeave()
    {
        texture->Unbind();
        shader->Unbind();
    }
}
