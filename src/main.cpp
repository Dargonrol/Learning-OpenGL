#define GLFW_INCLUDE_NONE

#include "Renderer.h"
#include "Scene/SceneManager.h"
#include "Scene/Scenes/Scene_ClearColor.h"
#include "Scene/Scenes/Scene_Menu.h"
#include "Scene/Scenes/Scene_Texture2D.h"

int main()
{
    Renderer renderer(650, 500);
    renderer.SetTitle("OpenGL Test");
    renderer.Init();

    Scene::SceneManager sceneManager{renderer};
    sceneManager.RegisterScene<Scene::Scene_Menu>("Menu");
    sceneManager.RegisterScene<Scene::Scene_ClearColor>("Clear Color");
    sceneManager.RegisterScene<Scene::Scene_Texture2D>("Texture2D");
    sceneManager.SetScene<Scene::Scene_Menu>();

    while (!renderer.GetWindowShouldClose())
    {
        // set up some kind of cache to see if shader is already bound. if so, don't run openGL call

        sceneManager.Update(0.0f);

        sceneManager.Render();
    }

    renderer.Shutdown();
    exit(EXIT_SUCCESS);
}
