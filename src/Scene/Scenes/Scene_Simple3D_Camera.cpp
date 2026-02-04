#include "Scene_Simple3D_Camera.h"

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
            BASE_PATH / "resources/shaders/Simple3D/Simple3D_VERT.shader",
            BASE_PATH / "resources/shaders/Simple3D/Simple3D_FRAG.shader"
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

        for(int i = 0; i < 10; ++i)
        {
            m_model[i] = glm::mat4(1.0f);
            m_model[i] = glm::translate(m_model[i], cubePositions[i]);
        }

        m_camera = std::make_unique<Camera>();

        m_camera->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
        m_camera->enableMouseControl = true;

        m_camSpeed = 10.0f;
        m_camSensitivity = 4.0f;
        m_mouseSensitivity = 0.002f;

        m_camera->SetMouseSensitivity(m_mouseSensitivity);

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
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_camera->GetProjectionMatrix());

        for (const auto & i : m_model)
        {
            m_shader->SetUniformMat4f("u_model", i);
            renderer_->Draw(*m_va, *m_shader, 36); // will eventually be va, ib, material
        }
    }

    void Scene_Simple3D_Camera::ImGuiRender()
    {
        m_cameraModeIndex = static_cast<int>(m_camera->GetMode());

        ImGui::Begin("Camera Control");

        if (ImGui::Button("back"))
            sceneManager_->SetScene("Menu");

        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::Button("reset Camera"))
            ResetCamera();

        if (ImGui::Combo("Camera Mode", &m_cameraModeIndex, Camera::GetCameraModes().data(), Camera::GetCameraModes().size()))
            m_camera->SetMode(static_cast<CameraMode>(m_cameraModeIndex));

        ImGui::SliderFloat("Flying Speed", &m_camSpeed, 0.1f, 100.0f);
        ImGui::SliderFloat("Mouse Sensitivity", &m_mouseSensitivity, 0.0005f, 0.009f);

        ImGui::Text("Direction Vector: X: %.3f Y: %.3f Z: %.3f", m_camera->GetDirectionVector().x, m_camera->GetDirectionVector().y, m_camera->GetDirectionVector().z);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void Scene_Simple3D_Camera::OnEnter()
    {
        m_shader->Bind();
        m_tex1->Bind(0);
        m_tex2->Bind(1);
        GLCall(glEnable(GL_DEPTH_TEST));
        double x, y;
        glfwGetCursorPos(&renderer_->GetWindow(), &x, &y);
        m_camera->SyncMouse(x, y);
    }

    void Scene_Simple3D_Camera::OnLeave()
    {
        m_tex1->Unbind();
        m_tex2->Unbind();
        m_shader->Unbind();
        GLCall(glDisable(GL_DEPTH_TEST));
        renderer_->ReleaseMouse();
    }

    void Scene_Simple3D_Camera::HandleInput(const float deltaTime)
    {
        GLFWwindow* window = &renderer_->GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sceneManager_->SetScene<Scene_Menu>();
            return;
        }

        m_camera->SetMouseSensitivity(m_mouseSensitivity);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            m_camera->Reset();
        if (m_prevTabState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
        {
            if (m_camera->GetMode() == CameraMode::ORBIT)
                m_camera->SetMode(CameraMode::FPS);
            else
                m_camera->SetMode(CameraMode::ORBIT);
        }

        m_camera->HandleGenericCameraControls(window, deltaTime, m_camSpeed, m_camSensitivity);

        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) | glfwGetKey(window, GLFW_KEY_E)) == GLFW_PRESS)
            m_camera->AddRoll(m_camSensitivity * 0.5f * deltaTime);
        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) | glfwGetKey(window, GLFW_KEY_Q)) == GLFW_PRESS)
            m_camera->AddRoll(-m_camSensitivity * 0.5f * deltaTime);

        if (m_camera->GetMode() != CameraMode::FPS || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        {
            renderer_->ReleaseMouse();
            m_camera->enableMouseControl = false;
        }
        else
        {
            renderer_->CaptureMouse();
            m_camera->enableMouseControl = true;
        }

        m_prevTabState = glfwGetKey(window, GLFW_KEY_TAB);
    }

    void Scene_Simple3D_Camera::OnResize(int width, int height)
    {
        m_camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }
}
