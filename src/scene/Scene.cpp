#include "Scene.h"

#include "core/Application.h"
#include "core/SceneManager.h"
#include "renderer/Renderer.h"

namespace Scene
{
    void Scene::OnLeave()
    {
        sm_->GetRenderer().app.SetTitle("OpenGL App");
    }
}
