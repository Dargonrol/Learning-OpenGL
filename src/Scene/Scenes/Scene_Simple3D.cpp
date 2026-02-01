#include "Scene_Simple3D.h"

#include "imgui.h"
#include "../../OpenGL.h"

#include "Scene_Menu.h"
#include "../../Core/Util.h"
#include "../../Core/Shader.h"
#include "../../Core/VertexBufferLayout.h"
#include "../../Core/IndexBuffer.h"
#include "../../Core/Texture.h"
#include "../../Core/VertexArray.h"
#include "../../Core/VertexBuffer.h"
#include "../SceneManager.h"
#include "../../Core/Renderer.h"
#include "../../Extra/Camera.h"
#include "glm/gtc/type_ptr.hpp"

namespace Scene
{
    Scene_Simple3D::Scene_Simple3D() = default;
    Scene_Simple3D::~Scene_Simple3D() = default;

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
        //m_shader->SetUniformMat4f("u_model", m_model);
        //m_shader->SetUniformMat4f("u_view", m_view);
        //m_shader->SetUniformMat4f("u_proj", m_proj);

        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        m_vb = std::make_unique<VertexBuffer>(vertices, 6 * 6 * 5 * sizeof(float));
        m_vb->Bind();

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);

        m_va = std::make_unique<VertexArray>();
        m_va->AddBuffer(*m_vb, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 6);
        m_ib->Bind();

        m_shader->Bind();
        m_tex1 = std::make_unique<Texture>(BASE_PATH / "resources/textures/base.png");
        m_tex1->Bind(0);
        m_shader->SetUniform1i("u_Texture1", 0);

        m_tex2 = std::make_unique<Texture>(BASE_PATH / "resources/textures/Nian.png");
        m_tex2->Bind(1);
        m_shader->SetUniform1i("u_Texture2", 1);

        int loc1 = glGetUniformLocation(m_shader->GetRendererID(), "u_Texture1");
        int loc2 = glGetUniformLocation(m_shader->GetRendererID(), "u_Texture2");

        for(unsigned int i = 0; i < 10; i++)
        {
            m_model[i] = {1.0f};
        }

        m_camera = std::make_unique<Camera>();
        m_proj = {};

        //m_model = glm::rotate(m_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, -3.0f));
        m_proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_radius = 10.0f;
        m_speed = 0.5f;
        m_time = 0.0f;
        m_camX = 0.0f;
        m_camZ = 0.0f;
        m_angle = 0.0f;

        m_camera->SetMode(CameraMode::ORBIT);

        return error;
    }

    void Scene_Simple3D::Update(float deltaTime)
    {
        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();
        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (keyState == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene<Scene_Menu>();
            return;
        }

        for(unsigned int i = 0; i < 10; i++)
        {
            m_model[i] = glm::mat4(1.0f);
            m_model[i] = glm::translate(m_model[i], cubePositions[i]);
            rotationAngles[i] += 20.0f * (i + 0.5) * deltaTime;
            m_model[i] = glm::rotate(m_model[i], glm::radians(rotationAngles[i]), glm::vec3(1.0f, 0.3f, 0.5f));
        }

        m_angle = m_time * m_speed;
        m_camX = std::sin(m_angle) * m_radius;
        m_camZ = std::cos(m_angle) * m_radius;

        m_camera->SetPosition({m_camX, 0.0, m_camZ});
        m_camera->Update();
        m_time += deltaTime;
    }

    void Scene_Simple3D::Render()
    {
        const auto& renderer = p_SceneManager_Ref->GetRenderer();
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_proj);

        for(unsigned int i = 0; i < 10; i++)
        {
            m_shader->SetUniformMat4f("u_model", m_model[i]);
            renderer.Draw(*m_va, *m_shader, 36); // will eventually be va, ib, material
        }
    }

    void Scene_Simple3D::ImGuiRender()
    {
        ImGui::Begin("Camera Control");
        if (ImGui::Button("back"))
            p_SceneManager_Ref->SetScene("Menu");
        ImGui::SliderFloat("Speed", &m_speed, -10.0f, 10.0f);
        ImGui::SliderFloat("Radius", &m_radius, -20.0f, 20.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void Scene_Simple3D::OnEnter()
    {
        m_shader->Bind();
        m_tex1->Bind(0);
        m_tex2->Bind(1);
        GLCall(glEnable(GL_DEPTH_TEST));
    }

    void Scene_Simple3D::OnLeave()
    {
        m_tex1->Unbind();
        m_tex2->Unbind();
        m_shader->Unbind();
        GLCall(glDisable(GL_DEPTH_TEST));
    }
}
