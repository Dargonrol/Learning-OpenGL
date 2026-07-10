#pragma once
#include <glm/mat4x4.hpp>
#include <filesystem>

#include "Core/ResourceManager.h"
#include "Core/Render/Model.h"

using rm = ResourceManager;

class GameObject
{
public:
    GameObject() {}
    GameObject(const Handle handle) : modelHandle_(handle) {}
    GameObject(const std::filesystem::path& path)
    {
        if (!exists(path))
            return;

        if (const Handle modelHandle = rm::Get().modelPool.GetHandle(path.string()))
        {
            modelHandle_ = modelHandle;
            return;
        }

        modelHandle_ = rm::Get().modelPool.Register(path.string(), Model(path));
    }
    ~GameObject() {}

    [[nodiscard]] Handle GetModelHandle() const { return modelHandle_; }


public:
    glm::mat4 modelMatrix{1.0f};

private:
    Handle modelHandle_{0, 0};
};
