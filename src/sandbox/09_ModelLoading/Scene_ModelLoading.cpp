#include "Scene_ModelLoading.h"

#include "renderer/IndexBuffer.h"
#include "renderer/Material.h"
#include "scene/Mesh.h"
#include "scene/Model.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "imgui.h"
#include "core/Application.h"
#include "core/SceneManager.h"
#include "scene/GameObject.h"
#include "scene/LightObject.h"

namespace Scene
{
    using rm = ResourceManager;

    Scene_ModelLoading::Scene_ModelLoading() = default;
    Scene_ModelLoading::~Scene_ModelLoading() = default;

    int Scene_ModelLoading::Init()
    {
        renderer_ = &sm_->GetRenderer();
        int error = 0;

        m_camSpeed = 5.0f;
        m_camSensitivity = 4.0f;
        m_mouseSensitivity = 0.002f;

        LightObject obj(BASE_PATH / "resources/models/primitives/Cube/Cube.obj", rm_);
        lights_.emplace_back(obj);
        lights_[0].SetMaterialOverride(Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error));

        camera_ = std::make_unique<Camera>(CameraMode::FPS);

        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetTarget({0.0f, 0.0f, 0.0f});

        glm::vec3& pos = camera_->GetPosition();
        const glm::vec3& target = camera_->GetTargetPos();

        camera_->SetMode(CameraMode::FPS);

        glm::vec3 initialDir = glm::normalize(target - pos);
        float yaw = std::atan2(initialDir.z, initialDir.x);
        float pitch = glm::asin(initialDir.y);

        camera_->SetYaw(yaw);
        camera_->SetPitch(pitch);
        camera_->SetAspectRatio(static_cast<float>(sm_->GetRenderer().app.GetWindowWidth()) / static_cast<float>(sm_->GetRenderer().app.GetWindowHeight()));
        camera_->enableMouseControl = true;
        lights_[0].modelMatrix = glm::translate(lights_[0].modelMatrix, glm::vec3{1.0f, 1.0f, 2.0f});
        lights_[0].modelMatrix = glm::scale(lights_[0].modelMatrix, glm::vec3(0.5f));
        lights_[0].lightData.lightType = LightData::LightType::POINT;
        lights_[0].lightData.ambient = {1.0f, 1.0f, 1.0f};
        lights_[0].lightData.diffuse = {1.0f, 1.0f, 1.0f};
        lights_[0].lightData.specular = {0.5f, 0.5f, 0.5f};
        lights_[0].lightData.constant = 1.0f;
        lights_[0].lightData.linear = 0.3f;
        lights_[0].lightData.quadratic = 0.3f;
        lights_[0].lightData.position = lights_[0].modelMatrix[3];

        Model model(BASE_PATH / "resources/models/custom/backpack/backpack.obj", rm_);
        Handle h = rm_->modelPool.Register("backpack", std::move(model));
        object_ = std::make_unique<GameObject>(h);
        object_->modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));

        return error;
    }

    void Scene_ModelLoading::Update(float deltaTime)
    {
        HandleInput(deltaTime);
        camera_->Update(deltaTime);

        lights_[0].lightData.diffuse = lightColor_;
        lights_[0].lightData.specular = lightColor_;

        Material* materialLight = rm_->materialPool.Get(rm_->modelPool.Get(lights_[0].GetModelHandle())->GetSubMeshes()[0].materialHandle);
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

        renderer.Draw(*object_, *camera_, lights_);
        renderer.DrawLight(lights_[0], *camera_);
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
        lights_[0].SetMaterialOverride(Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error));
        double x, y;
        glfwGetCursorPos(&renderer_->GetWindow(), &x, &y);
        camera_->SyncMouse(x, y);
    }

    void Scene_ModelLoading::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
        renderer_->app.ReleaseMouse();
    }

    void Scene_ModelLoading::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(renderer_->app.GetWindowWidth()) / static_cast<float>(renderer_->app.GetWindowHeight()));
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
            renderer_->app.ReleaseMouse();
            camera_->enableMouseControl = false;
        }
        else
        {
            renderer_->app.CaptureMouse();
            camera_->enableMouseControl = true;
        }
    }
}
