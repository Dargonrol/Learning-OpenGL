#include "Scene_BasicPlane.h"

#include "../../Core/IncludeAll.h"
#include "../../Extra/Camera.h"

namespace Scene
{
    Scene_BasicPlane::Scene_BasicPlane() = default;
    Scene_BasicPlane::~Scene_BasicPlane() = default;

    int Scene_BasicPlane::Init()
    {
        int error = 0;

        ShaderFilePath paths = {
            BASE_PATH / "resources/shaders/BasicPlane/BasicPlane_VERT.shader",
            BASE_PATH / "resources/shaders/BasicPlane/BasicPlane_FRAG.shader"
        };

        float vertices[] = {
            -50.0f, -50.0f,
             50.0f, -50.0f,
             50.0f,  50.0f,
            -50.0f,  50.0f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_shader = std::make_unique<Shader>(paths, error);
        if (error)
            return error;
        m_shader->Bind();

        m_ib = std::make_unique<IndexBuffer>(indices, 3 * 2);
        m_ib->Bind();

        m_vb = std::make_unique<VertexBuffer>(vertices, 2 * 4 * sizeof(float));
        m_vb->Bind();

        VertexBufferLayout layout;
        layout.Push<float>(2);

        m_va = std::make_unique<VertexArray>();
        m_va->Bind();
        m_va->AddBuffer(*m_vb, layout);

        Renderer* renderer = &sceneManager_->GetRenderer();

        m_camera = std::make_unique<Camera>(CameraMode::ORTHO);

        m_model = glm::translate(glm::mat4(1.0f), {200, 200, 0});
        m_camera->SetOrthoData({{0.0f, (float)renderer->GetWindowWidth(), 0.0f, (float)renderer->GetWindowHeight()}, -1.0f, 1.0f});

        m_shader->SetUniformMat4f("u_model", m_model);
        m_shader->SetUniformMat4f("u_proj", m_camera->GetProjectionMatrix());
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());

        return error;
    }

    void Scene_BasicPlane::Update(float deltaTime)
    {
        GLFWwindow* window = &sceneManager_->GetRenderer().GetWindow();

        m_camera->Update();
        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (keyState == GLFW_PRESS) {
            sceneManager_->SetScene<Scene_Menu>();
        }
        m_camera->HandleGenericCameraControls(window, deltaTime, 200.0f);
    }

    void Scene_BasicPlane::Render()
    {
        if (!sceneManager_) return;
        const auto& renderer = sceneManager_->GetRenderer();

        //m_view = m_camera->GetViewMatrix();

        m_shader->Bind();
        m_shader->SetUniformMat4f("u_model", m_model);
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_camera->GetProjectionMatrix());

        renderer.Draw(*m_va, *m_ib, *m_shader); // will eventually be va, ib, material
    }

    void Scene_BasicPlane::ImGuiRender()
    {
    }

    void Scene_BasicPlane::OnEnter()
    {
        Scene::OnEnter();
    }

    void Scene_BasicPlane::OnLeave()
    {
        Scene::OnLeave();
    }

    void Scene_BasicPlane::OnResize(int width, int height)
    {
        m_camera->SetOrthoBounds({0.0f, (float)width, 0.0f, (float)height});
    }
}
