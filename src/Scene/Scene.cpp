#include "Scene.h"
#include "SceneManager.h"
#include "../Core/Renderer.h"

namespace Scene
{
    void Scene::OnLeave()
    {
        sceneManager_->GetRenderer().SetTitle(sceneManager_->GetRenderer().GetDefaultTitle());
    }
}
