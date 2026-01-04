#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Scene.h"
#include "../Renderer.h"

namespace Scene
{
    class SceneManager
    {
    public:
        SceneManager(Renderer& renderer) : m_renderer(renderer) {};
        ~SceneManager() = default;

        template<typename T> requires (std::is_base_of_v<Scene, T>)
        void RegisterScene(const std::string_view name)
        {
            std::string SceneName = std::string(name);
            auto iterName = m_NameToHash_Map.find(SceneName);
            if(iterName != m_NameToHash_Map.end()) {
                return;
            }
            auto scene = std::make_unique<T>();
            scene->p_SceneManager_Ref = this;
            scene->Init();
            m_Scenes_Vector.push_back(std::move(scene));
            m_NameToHash_Map.emplace(SceneName, typeid(T).hash_code());
        }

        template<typename T> requires (std::is_base_of_v<Scene, T>)
        void UnRegisterScene()
        {
            size_t hash = typeid(T).hash_code();
            bool found = false;
            for (auto it = m_Scenes_Vector.begin(); it != m_Scenes_Vector.end(); ++it)
            {
                if (typeid(**it).hash_code() == hash)
                {
                    found = true;
                    m_Scenes_Vector.erase(it);
                    break;
                }
            }

            if (!found)
                return;

            for (auto it = m_NameToHash_Map.begin(); it != m_NameToHash_Map.end(); ++it)
            {
                if (it->second == hash)
                {
                    m_NameToHash_Map.erase(it);
                    return;
                }
            }
        }
        void UnRegisterScene(std::string_view name);

        void SetScene(std::string_view name);

        template<typename T> requires (std::is_base_of_v<Scene, T>)
        void SetScene()
        {
            size_t hash = typeid(T).hash_code();
            for (auto& item : m_Scenes_Vector)
            {
                if (typeid(*item).hash_code() == hash)
                {
                    m_CurrentScene = item.get();
                    return;
                }
            }
        }

        void Render() const;
        void Update(float deltaTime) const;

        [[nodiscard]] std::vector<std::string> GetSceneNames() const;
        [[nodiscard]] Scene& GetCurrentScene() const { return *m_CurrentScene; }
        [[nodiscard]] const Renderer& GetRenderer() const { return m_renderer; }

    private:
        std::unordered_map<std::string, size_t> m_NameToHash_Map;
        std::vector<std::unique_ptr<Scene>> m_Scenes_Vector;

        Scene* m_CurrentScene = nullptr;
        Renderer& m_renderer;
    };
}
