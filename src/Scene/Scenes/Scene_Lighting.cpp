#include "Scene_Lighting.h"

#include "../../Core/IncludeAll.h"
#include "../../Extra/Camera.h"

namespace Scene
{
    Scene_Lighting::Scene_Lighting() = default;
    Scene_Lighting::~Scene_Lighting() = default;

    int Scene_Lighting::Init()
    {
        int error = 0;

        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/Lighting/Lighting_VERT.shader",
            BASE_PATH / "resources/shaders/Lighting/Lighting_FRAG.shader"
        };

        m_shader = std::make_unique<Shader>(paths, error);
        if (error) return error;
        m_shader->Bind();

        float vertices[] = {
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.0f, -0.5f, -0.5f
        };

        const unsigned int indices[] = {
            0, 1, 2
        };

        VertexBufferLayout layout;
        layout.Push<float>(3);

        m_va = std::make_unique<VertexArray>();
        m_va->Bind();

        m_vbCube = std::make_unique<VertexBuffer>(vertices, 3 * 3 * sizeof(float));
        m_vbCube->Bind();
        m_va->AddBuffer(*m_vbCube, layout);

        m_ib = std::make_unique<IndexBuffer>(indices, 3);
        m_ib->Bind();

        const auto& renderer = sceneManager_->GetRenderer();
        m_camera = std::make_unique<Camera>(CameraMode::FPS);
        m_camera->SetPosition({0.0f, 0.0f, 3.0f});
        m_camera->SetAspectRatio(static_cast<float>(renderer.GetWindowWidth()) / static_cast<float>(renderer.GetWindowHeight()));

        m_modelCube = {1.0f};
        m_modelCube = glm::translate(m_modelCube, glm::vec3{0.0f, 0.0f, 0.0f});

        return error;
    }

    void Scene_Lighting::Update(float deltaTime)
    {
        HandleInput(deltaTime);
        m_camera->Update();
    }

    void Scene_Lighting::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& renderer = sceneManager_->GetRenderer();
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_camera->GetProjectionMatrix());
        m_shader->SetUniformMat4f("u_model", m_modelCube);
        renderer.Draw(*m_va, *m_ib, *m_shader);
    }

    void Scene_Lighting::ImGuiRender()
    {
        m_cameraModeIndex = static_cast<int>(m_camera->GetMode());

        ImGui::Begin("Camera Control");

        if (ImGui::Button("back"))
            sceneManager_->SetScene("Menu");

        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::Button("reset Camera"))
            m_camera->Reset();

        if (ImGui::Combo("Camera Mode", &m_cameraModeIndex, Camera::GetCameraModes().data(), Camera::GetCameraModes().size()))
            m_camera->SetMode(static_cast<CameraMode>(m_cameraModeIndex));

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void Scene_Lighting::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
        Scene::OnEnter();
    }

    void Scene_Lighting::OnLeave()
    {
        m_shader->Unbind();
        GLCall(glDisable(GL_DEPTH_TEST));
        glfwSetInputMode(&sceneManager_->GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        Scene::OnLeave();
    }

    void Scene_Lighting::HandleInput(float deltaTime)
    {
        GLFWwindow* window = &sceneManager_->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sceneManager_->SetScene("Menu");
        }

        m_camera->HandleGenericCameraControls(window, deltaTime);

        if (m_camera->GetMode() != CameraMode::FPS || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}
