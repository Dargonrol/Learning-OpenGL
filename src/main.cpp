#define GLFW_INCLUDE_NONE

#include "Core/Renderer.h"
#include "Scene/SceneManager.h"
#include "Scene/Scenes/Scene_BasicPlane.h"
#include "Scene/Scenes/Scene_ClearColor.h"
#include "Scene/Scenes/Scene_Lighting.h"
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
    sceneManager.RegisterScene<Scene::Scene_ClearColor>("01 Clear Color"sv);
    sceneManager.RegisterScene<Scene::Scene_BasicPlane>("02 Basic Plane");
    sceneManager.RegisterScene<Scene::Scene_Texture2D>("03 Texture2D"sv);
    sceneManager.RegisterScene<Scene::Scene_Simple3D>("04 Simple3D"sv);
    sceneManager.RegisterScene<Scene::Scene_Simple3D_Camera>("05 Simple3D Camera"sv);
    sceneManager.RegisterScene<Scene::Scene_Lighting>("06 Lighting"sv);
    sceneManager.SetScene<Scene::Scene_Menu>();

    while (!renderer.GetWindowShouldClose())
    {
        sceneManager.Update(sceneManager.GetDeltaTime());
        sceneManager.Render();
    }

    renderer.Shutdown();
    exit(EXIT_SUCCESS);
}
