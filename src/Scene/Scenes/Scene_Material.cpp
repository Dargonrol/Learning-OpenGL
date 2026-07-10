#include "Scene_Material.h"

#include "Core/Render/Material.h"
#include "Core/ResourceManager.h"
#include "imgui.h"
#include "Scene/SceneManager.h"
#include "Core/IncludeAll.h"
#include "Extra/Camera.h"
#include "Extra/LightObject.h"
#include "Extra/GameObject.h"

namespace Scene
{
    int Scene_Material::Init()
    {
        int error = 0;
        light_ = std::make_unique<LightObject>(BASE_PATH / "resources/models/primitives/Cube/Cube.obj", rm_);
        cube_ = std::make_unique<GameObject>(BASE_PATH / "resources/models/primitives/Cube/Cube.obj", rm_);
        Model* lightModel = rm_->modelPool.Get(light_->GetModelHandle());
        Model* cubeModel = rm_->modelPool.Get(cube_->GetModelHandle());

        lightModel->GetSubMeshes()[0].materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error, true);
        cubeModel->GetSubMeshes()[0].materialHandle = Material::parseMaterial("default", BASE_PATH / "resources/materials/default.mat", *rm_, error, true);
        debugMaterial = Material::parseMaterial("debug", BASE_PATH / "resources/materials/debug.mat", *rm_, error);

        camera_ = std::make_unique<Camera>(CameraMode::ORBIT);
        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetAspectRatio(static_cast<float>(sm_->GetRenderer().GetWindowWidth()) / static_cast<float>(sm_->GetRenderer().GetWindowHeight()));
        camera_->enableMouseControl = true;

        cube_->modelMatrix = glm::translate(cube_->modelMatrix, glm::vec3{0.0f, 0.0f, 0.0f});
        light_->modelMatrix = glm::translate(light_->modelMatrix, glm::vec3{0.0f, 1.0f, 2.0f});
        light_->modelMatrix = glm::scale(light_->modelMatrix, glm::vec3(0.5f));

        return error;
    }

    void Scene_Material::Update(float deltaTime)
    {
        camera_->Update(deltaTime);

        Model* lightModel = rm_->modelPool.Get(light_->GetModelHandle());
        rm_->materialPool.Get(lightModel->GetSubMeshes()[0].materialHandle)->diffuse = lightColor_;
        rm_->materialPool.Get(lightModel->GetSubMeshes()[0].materialHandle)->specular = lightColor_;

        GLFWwindow* window = &sm_->GetRenderer().GetWindow();
        camera_->HandleGenericCameraControls(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sm_->SetScene("Menu");
        }
    }

    void Scene_Material::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& renderer = sm_->GetRenderer();

        renderer.Draw(*cube_, *camera_, {*light_});
        renderer.DrawLight(*light_, *camera_);

        glm::mat4 MVP_Cube = camera_->GetProjectionMatrix() * camera_->GetViewMatrix() * cube_->modelMatrix;
        glm::mat4 MVP_Light = camera_->GetProjectionMatrix() * camera_->GetViewMatrix() * light_->modelMatrix;
        //renderer.WireDraw(*cube_, MVP_Cube);
        //renderer.WireDraw(*light_, MVP_Light);
    }

    void Scene_Material::ImGuiRender()
    {
        ImGui::Begin("Material");
        ImGui::ColorPicker3("Light Color", &lightColor_[0]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("back"))
            sm_->SetScene("Menu");
        ImGui::End();
    }

    void Scene_Material::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        int error = 0;
        Model* lightModel = rm_->modelPool.Get(light_->GetModelHandle());
        Model* cubeModel = rm_->modelPool.Get(cube_->GetModelHandle());
        lightModel->GetSubMeshes()[0].materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error, true);
        cubeModel->GetSubMeshes()[0].materialHandle = Material::parseMaterial("default", BASE_PATH / "resources/materials/default.mat", *rm_, error, true);
    }

    void Scene_Material::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
        /*
        rm_->shaderPool.Remove(rm_->materialPool.Get(cube_->materialHandle)->shaderHandle);
        rm_->materialPool.Remove(cube_->materialHandle);
        rm_->shaderPool.Remove(rm_->materialPool.Get(light_->materialHandle)->shaderHandle);
        rm_->materialPool.Remove(light_->materialHandle);
        */
    }

    void Scene_Material::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(sm_->GetRenderer().GetWindowWidth()) / static_cast<float>(sm_->GetRenderer().GetWindowHeight()));
    }
}
