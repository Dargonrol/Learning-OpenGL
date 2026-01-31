#include "SceneManager.h"

#include <iostream>

namespace Scene
{
    void SceneManager::UnRegisterScene(const std::string_view name)
    {
        const std::string SceneName = std::string(name);
        const auto iterName = m_NameToHash_Map.find(SceneName);
        if(iterName == m_NameToHash_Map.end()) {
            return;
        }

        const size_t hash = iterName -> second;

        m_NameToHash_Map.erase(iterName);

        for (auto it = m_Scenes_Vector.begin(); it != m_Scenes_Vector.end(); ++it)
        {
            if (typeid(**it).hash_code() == hash)
            {
                m_Scenes_Vector.erase(it);
                return;
            }
        }
    }

    void SceneManager::SetScene(std::string_view name)
    {
        std::string SceneName = std::string(name);
        const auto iterName = m_NameToHash_Map.find(SceneName);
        if(iterName == m_NameToHash_Map.end()) {
            return;
        }

        const size_t hash = iterName -> second;
        // if scene is already set
        if (m_CurrentScene && typeid(*m_CurrentScene).hash_code() == hash)
            return;

        for (auto it = m_Scenes_Vector.begin(); it != m_Scenes_Vector.end(); ++it)
        {
            if (typeid(**it).hash_code() == hash)
            {
                if (m_CurrentScene)
                    m_CurrentScene->OnLeave();
                m_CurrentScene = it->get();
                if (m_CurrentScene)
                    m_CurrentScene->OnEnter();
                return;
            }
        }
    }

    void SceneManager::Render() const
    {
        m_renderer.Clear();
        if (m_CurrentScene)
        {
            m_CurrentScene->Render();
            m_renderer.ImGui_BeginFrame();
            m_CurrentScene->ImGuiRender();
            m_renderer.ImGui_EndFrame();
        }
    }

    void SceneManager::Update(float deltaTime) const
    {
        m_renderer.Update();
        if (m_CurrentScene)
        {
            m_CurrentScene->Update(deltaTime);
        }
    }

    std::vector<std::string> SceneManager::GetSceneNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_Scenes_Vector.size());
        for (const auto& [key, value]: m_NameToHash_Map)
        {
            names.push_back(key);
        }
        return names;
    }

}


