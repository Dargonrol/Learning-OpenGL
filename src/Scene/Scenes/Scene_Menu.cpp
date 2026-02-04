#include "Scene_Menu.h"

#include <iostream>

#include "imgui.h"
#include "../SceneManager.h"
#include "../../Core/Renderer.h"
#include "GLFW/glfw3.h"

namespace Scene
{
    using namespace std::literals;
    int Scene_Menu::s_prevEscapeState;

    Scene_Menu::Scene_Menu() = default;
    Scene_Menu::~Scene_Menu() = default;

    void Scene_Menu::OnEnter()
    {
        s_prevEscapeState = glfwGetKey(&sceneManager_->GetRenderer().GetWindow(), GLFW_KEY_ESCAPE);
        sceneManager_->GetRenderer().SetTitle("Menu"sv);
    }

    void Scene_Menu::Render()
    {
    }

    void Scene_Menu::Update(float deltaTime)
    {
        GLFWwindow* window = &sceneManager_->GetRenderer().GetWindow();

        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (s_prevEscapeState == GLFW_RELEASE && keyState == GLFW_PRESS) {
            sceneManager_->GetRenderer().SetWindowShouldClose();
        }

        // Update previous state
        s_prevEscapeState = keyState;
    }

    void Scene_Menu::ImGuiRender()
    {
        ImGui::Begin("Menu");
        for (auto& scene: sceneManager_->GetSceneNames())
        {
            if (scene != "Menu" && ImGui::Button(scene.c_str()))
            {
                sceneManager_->SetScene(scene);
                ImGui::End();
                return;
            }
        }

        float buttonHeight = ImGui::GetFrameHeight();
        float avail = ImGui::GetContentRegionAvail().y;

        if (avail > buttonHeight)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + avail - buttonHeight - 5);

        ImGui::Separator();

        if (ImGui::Button("Reset Scenes"))
        {
            std::cout << "Resetting all scenes\n";
            for (auto& scene: sceneManager_->GetScenesVector())
            {
                if (scene->name == this->name)
                    continue;
                scene->initialized = false;
            }
        }
        ImGui::End();
    }
}
