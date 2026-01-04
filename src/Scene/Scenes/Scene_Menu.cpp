#include "Scene_Menu.h"

#include "imgui.h"
#include "../SceneManager.h"

namespace Scene
{
    void Scene_Menu::Render()
    {
    }

    void Scene_Menu::Update(float deltaTime)
    {
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
        ImGui::End();
    }
}
