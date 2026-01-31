#include "Scene_Menu.h"

#include <iostream>

#include "imgui.h"
#include "../SceneManager.h"

namespace Scene
{
    using namespace std::literals;
    int Scene_Menu::s_prevEscapeState;

    void Scene_Menu::OnEnter()
    {
        s_prevEscapeState = glfwGetKey(&p_SceneManager_Ref->GetRenderer().GetWindow(), GLFW_KEY_ESCAPE);
        p_SceneManager_Ref->GetRenderer().SetTitle("Menu"sv);
    }

    void Scene_Menu::Render()
    {
    }

    void Scene_Menu::Update(float deltaTime)
    {
        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        int keyState = glfwGetKey(window, GLFW_KEY_ESCAPE);

        if (s_prevEscapeState == GLFW_RELEASE && keyState == GLFW_PRESS) {
            p_SceneManager_Ref->GetRenderer().SetWindowShouldClose();
        }

        // Update previous state
        s_prevEscapeState = keyState;
    }

    void Scene_Menu::ImGuiRender()
    {
        ImGui::Begin("Menu");
        for (auto& scene: p_SceneManager_Ref->GetSceneNames())
        {
            if (scene != "Menu" && ImGui::Button(scene.c_str()))
            {
                p_SceneManager_Ref->SetScene(scene);
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
            for (auto& scene: p_SceneManager_Ref->GetScenesVector())
            {
                if (scene->name == this->name)
                    continue;
                scene->initialized = false;
            }
        }
        ImGui::End();
    }
}
