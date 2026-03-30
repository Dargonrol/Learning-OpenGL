#include "Scene_Material.h"

#include <iostream>

#include "../../Core/Material.h"
#include "../../Core/ResourceManager.h"
#include "imgui.h"
#include "../SceneManager.h"
#include "../../Core/IncludeAll.h"

namespace Scene
{
    int Scene_Material::Init()
    {
        int error = 0;
        Handle materialHandle = Material::parseMaterial("light", BASE_PATH / "resources/materials/light.mat", *rm_, error);

        if (rm_->materialPool.Exists(materialHandle))
            std::cout << *rm_->materialPool.Get(materialHandle) << "\n";

        return error;
    }

    void Scene_Material::Update(float deltaTime)
    {
        GLFWwindow* window = &sm_->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sm_->SetScene("Menu");
        }
    }

    void Scene_Material::Render()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }

    void Scene_Material::ImGuiRender()
    {
        ImGui::Begin("Clear Color Test");
        if (ImGui::Button("back"))
            sm_->SetScene("Menu");
        ImGui::End();
    }

    void Scene_Material::OnEnter()
    {

    }

    void Scene_Material::OnLeave()
    {

    }
}
