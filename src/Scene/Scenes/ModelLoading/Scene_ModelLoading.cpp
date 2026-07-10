#include "Scene_ModelLoading.h"

#include "Core/Render/IndexBuffer.h"
#include "Core/Render/Material.h"
#include "Core/Render/Mesh.h"
#include "Core/Render/Model.h"
#include "Core/Render/Shader.h"
#include "Core/Render/Texture.h"
#include "imgui.h"
#include "Scene/SceneManager.h"
#include "Core/Render/Model.h"

namespace Scene
{
    using rm = ResourceManager;

    Scene_ModelLoading::Scene_ModelLoading() = default;
    Scene_ModelLoading::~Scene_ModelLoading() = default;

    int Scene_ModelLoading::Init()
    {
        int error = 0;

        m_camSpeed = 5.0f;
        m_camSensitivity = 4.0f;
        m_mouseSensitivity = 0.002f;

        light_ = std::make_unique<Cube>();
        light_->materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error);

        camera_ = std::make_unique<Camera>(CameraMode::ORBIT);
        camera_->SetTarget({0.0, 0.0, 0.0});
        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetMode(CameraMode::FPS);
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
        camera_->enableMouseControl = true;
        light_->modelMatrix = glm::translate(light_->modelMatrix, glm::vec3{1.0f, 1.0f, 2.0f});
        light_->modelMatrix = glm::scale(light_->modelMatrix, glm::vec3(0.5f));
        light_->lightSource = true;
        light_->light.lightType = Light::LightType::POINT;
        light_->light.ambient = {1.0f, 1.0f, 1.0f};
        light_->light.diffuse = {1.0f, 1.0f, 1.0f};
        light_->light.specular = {0.5f, 0.5f, 0.5f};
        light_->light.constant = 1.0f;
        light_->light.linear = 0.3f;
        light_->light.quadratic = 0.3f;
        light_->light.position = light_->modelMatrix[3];

        model_ = std::make_unique<Model>(BASE_PATH / "resources/models/backpack/backpack.obj");

        modelMatrix_ = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));

        return error;
    }

    void Scene_ModelLoading::Update(float deltaTime)
    {
        HandleInput(deltaTime);
        camera_->Update(deltaTime);

        light_->light.diffuse = lightColor_;
        light_->light.specular = lightColor_;

        Material* materialLight = light_->GetMaterial(*rm_);
        if (materialLight)
        {
            materialLight->diffuse = lightColor_;
        }

        GLFWwindow* window = &sm_->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sm_->SetScene("Menu");
        }
    }

    void Scene_ModelLoading::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const auto& renderer = sm_->GetRenderer();

        Shader* shaderLight = light_->GetShader(*rm_);
        Material* materialLight = light_->GetMaterial(*rm_);

        for (const auto& subMesh: model_->GetSubMeshes())
        {
            Mesh* mesh          = rm::Get().meshPool.Get(subMesh.meshHandle);
            Material* material  = rm::Get().materialPool.Get(subMesh.materialHandle);
            Shader* shader      = rm::Get().shaderPool.Get(material->shaderHandle);


            shader->Bind();
            mesh->VAO.Bind();
            mesh->IBO.Bind();
            mesh->VBO.Bind();

            if (material->diffuseMap)
            {
                Texture* diffTex = rm::Get().texturePool.Get(material->diffuseMap);
                diffTex->Bind(0);
                shader->SetUniform1i("material.diffuse", 0);
            }
            if (material->specularMap)
            {
                Texture* specTex = rm::Get().texturePool.Get(material->specularMap);
                specTex->Bind(1);
                shader->SetUniform1i("material.specular", 1);
            }

            shader->SetUniformMat4f("uView", camera_->GetViewMatrix());
            shader->SetUniformMat4f("uProj", camera_->GetProjectionMatrix());
            shader->SetUniformMat4f("uModel", modelMatrix_);
            shader->SetUniformVec3("uViewPos", camera_->GetPosition());

            shader->SetUniformVec3("material.diffuseColor", material->diffuse);
            shader->SetUniform1f("material.shininess", material->shininess);

            light_->SetLightUniforms(*rm_, *shader, 0, Light::LightType::POINT);
            shader->SetUniform1i("uPointLightCount", 1);

            shader->SetUniformVec3("material.diffuseColor", materialLight->diffuse);
            glm::mat4 MVP = camera_->GetProjectionMatrix() * camera_->GetViewMatrix() * modelMatrix_;
            //renderer.WireDraw(*mesh, MVP);
            renderer.Draw(*mesh);
        }

        light_->BindAll(*rm_);
        shaderLight->SetUniformMat4f("uView", camera_->GetViewMatrix());
        shaderLight->SetUniformMat4f("uProj", camera_->GetProjectionMatrix());
        shaderLight->SetUniformMat4f("uModel", light_->modelMatrix);
        shaderLight->SetUniformVec3("uLightColor", materialLight->diffuse);
        renderer.Draw(*light_);
    }

    void Scene_ModelLoading::ImGuiRender()
    {
        ImGui::Begin("Model Loading");
        ImGui::ColorPicker3("Light Color", &lightColor_[0]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("back"))
            sm_->SetScene("Menu");
        ImGui::End();
    }

    void Scene_ModelLoading::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        int error = 0;
        light_->materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error, true);
        double x, y;
        glfwGetCursorPos(&renderer_->GetWindow(), &x, &y);
        camera_->SyncMouse(x, y);
    }

    void Scene_ModelLoading::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
        renderer_->ReleaseMouse();
    }

    void Scene_ModelLoading::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
    }

    void Scene_ModelLoading::HandleInput(const float deltaTime)
    {
        GLFWwindow* window = &renderer_->GetWindow();
        camera_->HandleGenericCameraControls(window, deltaTime, m_camSpeed, m_camSensitivity);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            m_camSpeed = 0.08f;
        else
            m_camSpeed = 10.0f;

        if (camera_->GetMode() != CameraMode::FPS || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        {
            renderer_->ReleaseMouse();
            camera_->enableMouseControl = false;
        }
        else
        {
            renderer_->CaptureMouse();
            camera_->enableMouseControl = true;
        }
    }
}
