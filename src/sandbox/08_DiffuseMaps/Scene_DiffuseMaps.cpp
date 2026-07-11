#include "Scene_DiffuseMaps.h"

#include <iostream>

#include "renderer/Material.h"
#include "core/ResourceManager.h"
#include "imgui.h"
#include "core/Application.h"
#include "core/SceneManager.h"
#include "scene/Camera.h"
#include "scene/LightObject.h"

namespace Scene
{
    int Scene_DiffuseMaps::Init()
    {
        int error = 0;
        light_ = std::make_unique<LightObject>(BASE_PATH / "resources/models/primitives/Cube/Cube.obj", rm_);
        cube_ = std::make_unique<GameObject>(BASE_PATH / "resources/models/primitives/Cube/Cube.obj", rm_);

        light_->SetMaterialOverride(Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error));
        cube_->SetMaterialOverride(Material::parseMaterial("crate", BASE_PATH / "resources/materials/crate.mat", *rm_, error));

        camera_ = std::make_unique<Camera>(CameraMode::ORBIT);
        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetAspectRatio(static_cast<float>(sm_->GetRenderer().app.GetWindowWidth()) / static_cast<float>(sm_->GetRenderer().app.GetWindowHeight()));
        camera_->enableMouseControl = true;

        cube_->modelMatrix = glm::translate(cube_->modelMatrix, glm::vec3{0.0f, 0.0f, 0.0f});
        light_->modelMatrix = glm::translate(light_->modelMatrix, glm::vec3{1.0f, 1.0f, 2.0f});
        light_->modelMatrix = glm::scale(light_->modelMatrix, glm::vec3(0.5f));
        light_->lightData.lightType = LightData::LightType::POINT;
        light_->lightData.ambient = {1.0f, 1.0f, 1.0f};
        light_->lightData.diffuse = {1.0f, 1.0f, 1.0f};
        light_->lightData.specular = {0.5f, 0.5f, 0.5f};
        light_->lightData.constant = 1.0f;
        light_->lightData.linear = 0.5f;
        light_->lightData.quadratic = 0.5f;
        light_->lightData.position = light_->modelMatrix[3];

        return error;
    }

    void Scene_DiffuseMaps::Update(float deltaTime)
    {
        camera_->Update(deltaTime);

        light_->lightData.diffuse = lightColor_;
        light_->lightData.specular = lightColor_;

        Handle overrideHandle = light_->GetMaterialHandleOverride(0);
        Material* materialLight = rm_->materialPool.Get(overrideHandle);
        if (materialLight)
        {
            materialLight->diffuse = lightColor_;
        }

        GLFWwindow* window = &sm_->GetRenderer().GetWindow();
        camera_->HandleGenericCameraControls(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sm_->SetScene("Menu");
        }
    }

    void Scene_DiffuseMaps::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& renderer = sm_->GetRenderer();
        renderer.Draw(*cube_, *camera_, {*light_});
        renderer.DrawLight(*light_, *camera_);
    }

    void Scene_DiffuseMaps::ImGuiRender()
    {
        ImGui::Begin("Diffuse/Specular Maps");
        ImGui::ColorPicker3("Light Color", &lightColor_[0]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("back"))
            sm_->SetScene("Menu");
        ImGui::End();
    }

    void Scene_DiffuseMaps::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        int error = 0;
        light_->SetMaterialOverride(Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error));
        cube_->SetMaterialOverride(Material::parseMaterial("crate", BASE_PATH / "resources/materials/crate.mat", *rm_, error));
    }

    void Scene_DiffuseMaps::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void Scene_DiffuseMaps::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(sm_->GetRenderer().app.GetWindowWidth()) / static_cast<float>(sm_->GetRenderer().app.GetWindowHeight()));
    }
} // Scene