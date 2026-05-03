#include "Scene_ModelLoading.h"

#include "imgui.h"
#include "../SceneManager.h"
#include "../../Core/Renderer.h"
#include "../../Core/IncludeAll.h"

#include "GLFW/glfw3.h"

namespace Scene
{
    int Scene_ModelLoading::Init()
    {
        int error = 0;
        return error;
    }

    void Scene_ModelLoading::Update(float deltaTime)
    {
        GLFWwindow* window = &sm_->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sm_->SetScene("Menu");
        }
    }

    void Scene_ModelLoading::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }

    void Scene_ModelLoading::ImGuiRender()
    {
        ImGui::Begin("Diffuse/Specular Maps");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("back"))
            sm_->SetScene("Menu");
        ImGui::End();
    }

    void Scene_ModelLoading::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void Scene_ModelLoading::OnLeave()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void Scene_ModelLoading::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
    }

}
