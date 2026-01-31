#include "Scene_Simple3D.h"

#include "Scene_Menu.h"
#include "../../Util.h"
#include "../../VertexBufferLayout.h"
#include "../SceneManager.h"
#include "glm/gtc/type_ptr.hpp"

namespace Scene
{
    int Scene_Simple3D::Init()
    {
        int error = 0;
        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/Simple3D.vertGL",
            BASE_PATH / "resources/shaders/Simple3D.fragGL"
        };

        m_shader = std::make_unique<Shader>(paths, error);
        if (error)
            return error;
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_model", m_model);
        m_shader->SetUniformMat4f("u_view", m_view);
        m_shader->SetUniformMat4f("u_proj", m_proj);

        float vertices[] = {
            // positions         // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
           -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
           -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
        };
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        m_vb = std::make_unique<VertexBuffer>(vertices, 4 * 5 * sizeof(float));
        m_vb->Bind();

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);

        m_va = std::make_unique<VertexArray>();
        m_va->AddBuffer(*m_vb, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 6);
        m_ib->Bind();

        m_tex1 = std::make_unique<Texture>(BASE_PATH / "resources/textures/base.jpg");
        m_tex1->Bind(0);
        m_shader->SetUniform1i("u_Texture1", 0);

        m_tex1 = std::make_unique<Texture>(BASE_PATH / "resources/textures/Nian.png");
        m_tex1->Bind(0);
        m_shader->SetUniform1i("u_Texture2", 1);

        return error;
    }

    void Scene_Simple3D::Update(float deltaTime)
    {
        m_model = glm::rotate(m_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, -3.0f));
        m_proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_shader->Bind();
        m_shader->SetUniformMat4f("u_model", m_model);
        m_shader->SetUniformMat4f("u_view", m_view);
        m_shader->SetUniformMat4f("u_proj", m_proj);

        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (keyState == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene<Scene_Menu>();
        }
    }

    void Scene_Simple3D::Render()
    {
        const auto& renderer = p_SceneManager_Ref->GetRenderer();
        m_shader->Bind();
        renderer.Draw(*m_va, *m_ib, *m_shader); // will eventually be va, ib, material
    }

    void Scene_Simple3D::ImGuiRender()
    {

    }
}
