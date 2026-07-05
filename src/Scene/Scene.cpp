#include "Scene.h"
#include "SceneManager.h"
#include "Core/Render/Renderer.h"

namespace Scene
{
    void Scene::OnLeave()
    {
        sm_->GetRenderer().SetTitle(sm_->GetRenderer().GetDefaultTitle());
    }
}
