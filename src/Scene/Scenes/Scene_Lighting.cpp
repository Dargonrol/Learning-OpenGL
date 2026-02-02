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
            // Positions
            -0.5f, -0.5f, -0.5f, // 0: left-bottom-back
             0.5f, -0.5f, -0.5f, // 1: right-bottom-back
             0.5f,  0.5f, -0.5f, // 2: right-top-back
            -0.5f,  0.5f, -0.5f, // 3: left-top-back
            -0.5f, -0.5f,  0.5f, // 4: left-bottom-front
             0.5f, -0.5f,  0.5f, // 5: right-bottom-front
             0.5f,  0.5f,  0.5f, // 6: right-top-front
            -0.5f,  0.5f,  0.5f  // 7: left-top-front
        };

        unsigned int indices[] = {
            // Back face
            0, 1, 2,
            2, 3, 0,
            // Front face
            4, 5, 6,
            6, 7, 4,
            // Left face
            4, 0, 3,
            3, 7, 4,
            // Right face
            1, 5, 6,
            6, 2, 1,
            // Bottom face
            4, 5, 1,
            1, 0, 4,
            // Top face
            3, 2, 6,
            6, 7, 3
        };

        VertexBufferLayout layout;
        layout.Push<float>(3);

        m_ib = std::make_unique<IndexBuffer>(indices, 3 * 2 * 6);
        m_ib->Bind();

        m_vbCube = std::make_unique<VertexBuffer>(vertices, 3 * 8 * sizeof(float));
        m_vbCube->Bind();
        //m_vbLight = std::make_unique<VertexBuffer>(vertices, 3 * 8 * sizeof(float));
        //m_vbLight->Bind();

        m_va = std::make_unique<VertexArray>();
        m_va->AddBuffer(*m_vbCube, layout);
        //m_va->AddBuffer(*m_vbLight, layout);

        m_camera = std::make_unique<Camera>();
        m_camera->SetMode(CameraMode::ORBIT);

        m_modelCube = glm::rotate(m_modelCube, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        m_modelLight = glm::rotate(m_modelLight, glm::radians(-45.0f), glm::vec3(0.0f, 5.0f, 5.0f));
        m_proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_shader->SetUniformMat4f("u_model", m_modelCube);
        //m_shader->SetUniformMat4f("u_model2", m_modelLight);
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_proj);

        return error;
    }

    void Scene_Lighting::Update(float deltaTime)
    {
        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene("Menu");
        }

        m_camera->HandleGenericCameraControls(window, deltaTime);
        m_camera->Update();
    }

    void Scene_Lighting::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& renderer = p_SceneManager_Ref->GetRenderer();
        m_shader->Bind();
        m_shader->SetUniformMat4f("u_view", m_camera->GetViewMatrix());
        m_shader->SetUniformMat4f("u_proj", m_proj);
        m_shader->SetUniformMat4f("u_model", m_modelCube);
        renderer.Draw(*m_va, *m_ib, *m_shader);
    }

    void Scene_Lighting::ImGuiRender()
    {
    }

    void Scene_Lighting::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        Scene::OnEnter();
    }

    void Scene_Lighting::OnLeave()
    {
        Scene::OnLeave();
    }
}
