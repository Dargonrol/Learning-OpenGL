#include "Scene_Simple3D_Camera.h"

#include <cmath>
#include <iostream>

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
#include "../../Extra/Camera.h"
#include "../../Core/Renderer.h"
#include "glm/gtc/type_ptr.hpp"

namespace Scene
{
    Scene_Simple3D_Camera::Scene_Simple3D_Camera() = default;
    Scene_Simple3D_Camera::~Scene_Simple3D_Camera() = default;

    int Scene_Simple3D_Camera::Init()
    {
        int error = 0;
        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/Simple3D.vertGL",
            BASE_PATH / "resources/shaders/Simple3D.fragGL"
        };

        m_shader = std::make_unique<Shader>(paths, error);
        if (error)
            return error;

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

        for(auto & i : m_model)
        {
            i = {1.0f};
        }

        m_camera = std::make_unique<Camera>();
        m_proj = {};

        //m_model = glm::rotate(m_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, -3.0f));
        m_proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_camSpeed = 10.0f;
        m_camSensitivity = 4.0f;
        m_mouseSensitivity = 0.002f;
        m_camera->SetMode(CameraMode::ORBIT);

        m_mouseLastX = 0;
        m_mouseLastY = 0;
        m_mouseX = 0;
        m_mouseY = 0;

        m_prevTabState = GLFW_RELEASE;

        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

        return error;
    }

    void Scene_Simple3D_Camera::ResetCamera()
    {
        m_camSpeed = 10.0f;
        m_camera->SetPosition({0.0f, 0.0f, 3.0f});
        m_camera->Reset();
    }

    void Scene_Simple3D_Camera::Update(float deltaTime)
    {
        HandleInput(deltaTime);

        m_camera->Update();

        for(unsigned int i = 0; i < 10; i++)
        {
            m_model[i] = glm::mat4(1.0f);
            m_model[i] = glm::translate(m_model[i], cubePositions[i]);
            rotationAngles[i] += 20.0f * (i + 0.1f) * deltaTime;
            m_model[i] = glm::rotate(m_model[i], glm::radians(rotationAngles[i]), glm::vec3(1.0f, 0.3f, 0.5f));
        }

    }

    void Scene_Simple3D_Camera::Render()
    {
        const auto& renderer = p_SceneManager_Ref->GetRenderer();
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_proj);

        for (unsigned int i = 0; i < 10; i++)
        {
            m_shader->SetUniformMat4f("u_model", m_model[i]);
            renderer.Draw(*m_va, *m_shader, 36); // will eventually be va, ib, material
        }
    }

    void Scene_Simple3D_Camera::ImGuiRender()
    {
        m_cameraModeIndex = static_cast<int>(m_camera->GetMode());

        ImGui::Begin("Camera Control");

        if (ImGui::Button("back"))
            p_SceneManager_Ref->SetScene("Menu");

        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::Button("reset Camera"))
            ResetCamera();

        if (ImGui::Combo("Camera Mode", &m_cameraModeIndex, Camera::GetCameraModes().data(), Camera::GetCameraModes().size()))
            m_camera->SetMode(static_cast<CameraMode>(m_cameraModeIndex));

        ImGui::SliderFloat("Flying Speed", &m_camSpeed, 0.1f, 100.0f);
        ImGui::SliderFloat("Mouse Sensitivity", &m_mouseSensitivity, 0.0005f, 0.009f);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void Scene_Simple3D_Camera::OnEnter()
    {
        m_shader->Bind();
        m_tex1->Bind(0);
        m_tex2->Bind(1);
        GLCall(glEnable(GL_DEPTH_TEST));
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    }

    void Scene_Simple3D_Camera::OnLeave()
    {
        m_tex1->Unbind();
        m_tex2->Unbind();
        m_shader->Unbind();
        GLCall(glDisable(GL_DEPTH_TEST));
        glfwSetInputMode(&p_SceneManager_Ref->GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    }

    void Scene_Simple3D_Camera::HandleInput(const float deltaTime)
    {
        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene<Scene_Menu>();
            return;
        }

        m_mouseLastX = m_mouseX;
        m_mouseLastY = m_mouseY;
        glfwGetCursorPos(window, &m_mouseX, &m_mouseY);

        double deltaX = m_mouseX - m_mouseLastX;
        double deltaY = m_mouseLastY - m_mouseY;

        glm::vec3 dir = m_camera->GetDirectionVector();
        glm::vec3 up = m_camera->GetUpVector();
        glm::vec3 right = glm::normalize(glm::cross(dir, up));

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            m_camera->Reset();
        if (m_prevTabState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
        {
            if (m_camera->GetMode() == CameraMode::ORBIT)
                m_camera->SetMode(CameraMode::FPS);
            else
                m_camera->SetMode(CameraMode::ORBIT);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera->AddPosition(dir * m_camSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera->AddPosition(-right * m_camSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera->AddPosition(-dir * m_camSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera->AddPosition(right * m_camSpeed * deltaTime);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_camera->AddPosition(up * m_camSpeed * deltaTime);
        if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) | glfwGetKey(window, GLFW_KEY_C) | glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) == GLFW_PRESS)
            m_camera->AddPosition(-up * m_camSpeed * deltaTime);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            m_camera->AddPitch(m_camSensitivity * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            m_camera->AddPitch(-m_camSensitivity * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            m_camera->AddYaw(-m_camSensitivity * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            m_camera->AddYaw(m_camSensitivity * deltaTime);

        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) | glfwGetKey(window, GLFW_KEY_E)) == GLFW_PRESS)
            m_camera->AddRoll(m_camSensitivity * 0.5f * deltaTime);
        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) | glfwGetKey(window, GLFW_KEY_Q)) == GLFW_PRESS)
            m_camera->AddRoll(-m_camSensitivity * 0.5f * deltaTime);

        if (m_camera->GetMode() == CameraMode::ORBIT || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_camera->AddYaw(static_cast<float>(deltaX) * m_mouseSensitivity);
            m_camera->AddPitch(static_cast<float>(deltaY) * m_mouseSensitivity);
        }

        m_prevTabState = glfwGetKey(window, GLFW_KEY_TAB);
    }
}
