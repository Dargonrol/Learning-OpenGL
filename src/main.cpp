#define GLFW_INCLUDE_NONE

#include "Core/Renderer.h"
#include "Scene/SceneManager.h"
#include "Scene/Scenes/Scene_ClearColor.h"
#include "Scene/Scenes/Scene_Menu.h"
#include "Scene/Scenes/Scene_Simple3D.h"
#include "Scene/Scenes/Scene_Simple3D_Camera.h"
#include "Scene/Scenes/Scene_Texture2D.h"

int main()
{
    using namespace std::literals;
    Renderer renderer(650, 500);
    renderer.SetDefaultTitle("OpenGL Test"sv);
    renderer.Init();

    Scene::SceneManager sceneManager{renderer};
    sceneManager.RegisterScene<Scene::Scene_Menu>("Menu"sv);
    sceneManager.RegisterScene<Scene::Scene_ClearColor>("1 Clear Color"sv);
    sceneManager.RegisterScene<Scene::Scene_Texture2D>("2 Texture2D"sv);
    sceneManager.RegisterScene<Scene::Scene_Simple3D>("3 Simple3D"sv);
    sceneManager.RegisterScene<Scene::Scene_Simple3D_Camera>("4 Simple3D Camera"sv);
    sceneManager.SetScene<Scene::Scene_Menu>();

    while (!renderer.GetWindowShouldClose())
    {
        sceneManager.Update(sceneManager.GetDeltaTime());
        sceneManager.Render();
    }

    renderer.Shutdown();
    exit(EXIT_SUCCESS);
}
