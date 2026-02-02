#include "Scene_ClearColor.h"

#include "imgui.h"
#include "../SceneManager.h"
#include "../../Core/Renderer.h"
#include "../../Core/Util.h"
#include "../../OpenGL.h"

namespace Scene
{
    Scene_ClearColor::Scene_ClearColor() : m_ClearColor{1.0f, 0.75f, 0.5f, 1.0f}
    {
    }

    Scene_ClearColor::~Scene_ClearColor() = default;

    void Scene_ClearColor::Update(float deltaTime)
    {
        GLFWwindow* window = &p_SceneManager_Ref->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            p_SceneManager_Ref->SetScene("Menu");
        }
    }

    void Scene_ClearColor::Render()
    {
        GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    }

    void Scene_ClearColor::ImGuiRender()
    {
        ImGui::Begin("Clear Color Test");
        if (ImGui::Button("back"))
            p_SceneManager_Ref->SetScene("Menu");
        ImGui::ColorPicker4("Clear Color", m_ClearColor);
        ImGui::End();
    }
}
