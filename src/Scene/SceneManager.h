#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Scene.h"
class Renderer;
class GLFWwindow;

namespace Scene
{
    class SceneManager
    {
    public:
        explicit SceneManager(Renderer& renderer);
        ~SceneManager();

        static void FrameBufferCallback(GLFWwindow* window, int width, int height);
        void OnResize(int width, int height) const;

        template<typename T> requires (std::is_base_of_v<Scene, T>)
        void RegisterScene(const std::string_view name)
        {
            std::string SceneName = std::string(name);
            auto iterName = m_NameToHash_Map.find(SceneName);
            if(iterName != m_NameToHash_Map.end()) {
                return;
            }
            auto scene = std::make_unique<T>();
            scene->sceneManager_ = this;
            scene->renderer_ = &m_renderer;
            scene->name = std::string(name);
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
            for (auto& scene : m_Scenes_Vector)
            {
                if (typeid(*scene) == typeid(T))
                {
                    if (!scene->initialized)
                    {
                        if (scene->Init() != 0)
                        {
                            scene->initialized = false;
                            return;
                        }
                        scene->initialized = true;
                    }
                    if (m_CurrentScene)
                        m_CurrentScene->OnLeave();
                    m_CurrentScene = scene.get();
                    m_CurrentScene->OnEnter();
                    return;
                }
            }
        }

        void Render() const;
        void Update(float deltaTime);

        [[nodiscard]] std::vector<std::string> GetSceneNames() const;
        [[nodiscard]] Scene& GetCurrentScene() const { return *m_CurrentScene; }
        [[nodiscard]] const std::vector<std::unique_ptr<Scene>>& GetScenesVector() const { return m_Scenes_Vector; }
        [[nodiscard]] Renderer& GetRenderer() const { return m_renderer; }
        [[nodiscard]] float GetDeltaTime() const { return m_deltaTime; }

    private:
        std::unordered_map<std::string, size_t> m_NameToHash_Map;
        std::vector<std::unique_ptr<Scene>> m_Scenes_Vector;

        Scene* m_CurrentScene = nullptr;
        Renderer& m_renderer;
        float m_deltaTime = 0.0f;
        float m_lastFrameTime = 0.0f;
        float m_currentTime = 0.0f;
    };
}
