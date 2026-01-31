#include "Scene.h"
#include "SceneManager.h"

namespace Scene
{
    void Scene::OnLeave()
    {
        p_SceneManager_Ref->GetRenderer().SetTitle(p_SceneManager_Ref->GetRenderer().GetDefaultTitle());
    }
}
