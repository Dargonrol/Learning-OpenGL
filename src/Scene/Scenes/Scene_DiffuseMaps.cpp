#include "Scene_DiffuseMaps.h"

#include <iostream>

#include "../../Core/Material.h"
#include "../../Core/ResourceManager.h"
#include "imgui.h"
#include "../SceneManager.h"
#include "../../Core/IncludeAll.h"
#include "../../Extra/Camera.h"
#include "../../Extra/Objects/Cube.h"

namespace Scene
{
    int Scene_DiffuseMaps::Init()
    {
        int error = 0;
        light_ = std::make_unique<Cube>();
        light_->materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error);
        cube_ = std::make_unique<Cube>();
        cube_->materialHandle = Material::parseMaterial("crate", BASE_PATH / "resources/materials/crate.mat", *rm_, error);
        debugMaterial = Material::parseMaterial("debug", BASE_PATH / "resources/materials/debug.mat", *rm_, error);

        camera_ = std::make_unique<Camera>(CameraMode::ORBIT);
        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
        camera_->enableMouseControl = true;

        cube_->modelMatrix = glm::translate(cube_->modelMatrix, glm::vec3{0.0f, 0.0f, 0.0f});
        light_->modelMatrix = glm::translate(light_->modelMatrix, glm::vec3{1.0f, 1.0f, 2.0f});
        light_->modelMatrix = glm::scale(light_->modelMatrix, glm::vec3(0.5f));

        return error;
    }

    void Scene_DiffuseMaps::Update(float deltaTime)
    {
        camera_->Update(deltaTime);

        rm_->materialPool.Get(light_->materialHandle)->diffuse = lightColor_;
        rm_->materialPool.Get(light_->materialHandle)->specular = lightColor_;

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
        Shader* shaderCube = cube_->GetShader(*rm_);
        Material* materialCube = cube_->GetMaterial(*rm_);
        Shader* shaderLight = light_->GetShader(*rm_);
        Material* materialLight = light_->GetMaterial(*rm_);

        cube_->BindAll(*rm_);
        shaderCube->SetUniformMat4f("uView", camera_->GetViewMatrix());
        shaderCube->SetUniformMat4f("uProj", camera_->GetProjectionMatrix());
        shaderCube->SetUniformMat4f("uModel", cube_->modelMatrix);
        shaderCube->SetUniformVec3("uViewPos", camera_->GetPosition());
        shaderCube->SetUniform1i("material.diffuse", 0);
        shaderCube->SetUniform1i("material.specular", 1);
        shaderCube->SetUniform1f("material.shininess", materialCube->shininess);
        shaderCube->SetUniformVec3("light.ambient", materialLight->ambient);
        shaderCube->SetUniformVec3("light.diffuse", materialLight->diffuse);
        shaderCube->SetUniformVec3("light.specular", materialLight->specular);
        shaderCube->SetUniformVec3("light.position", light_->modelMatrix[3]);
        renderer.Draw(*cube_);

        light_->BindAll(*rm_);
        shaderLight->SetUniformMat4f("uView", camera_->GetViewMatrix());
        shaderLight->SetUniformMat4f("uProj", camera_->GetProjectionMatrix());
        shaderLight->SetUniformMat4f("uModel", light_->modelMatrix);
        shaderLight->SetUniformVec3("uLightColor", materialLight->diffuse);
        renderer.Draw(*light_);
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
        light_->materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error, true);
        cube_->materialHandle = Material::parseMaterial("crate", BASE_PATH / "resources/materials/crate.mat", *rm_, error, true);
    }

    void Scene_DiffuseMaps::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void Scene_DiffuseMaps::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
    }
} // Scene