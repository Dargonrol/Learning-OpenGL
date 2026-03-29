#include "Scene.h"
#include "SceneManager.h"
#include "../Core/Renderer.h"

namespace Scene
{
    void Scene::OnLeave()
    {
        sm_->GetRenderer().SetTitle(sm_->GetRenderer().GetDefaultTitle());
    }
}
