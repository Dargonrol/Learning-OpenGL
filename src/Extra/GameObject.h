#pragma once
#include <glm/mat4x4.hpp>
#include <filesystem>

#include "Core/ResourceManager.h"
#include "Core/Render/Model.h"

using rm = ResourceManager;

class GameObject
{
public:
    GameObject() = default;
    explicit GameObject(const Handle handle) : modelHandle_(handle) {}
    GameObject(const std::filesystem::path& path, ResourceManager* rm)
    {
        if (!exists(path))
        {
            std::cerr << "[ERROR] Cannot find file: " << path << std::endl;
            return;
        }

        if (const Handle modelHandle = rm->modelPool.GetHandle(path.string()))
        {
            modelHandle_ = modelHandle;
            return;
        }

        modelHandle_ = rm->modelPool.Register(path.string(), Model(path, rm));
    }
    ~GameObject() = default;

    [[nodiscard]] Handle GetModelHandle() const { return modelHandle_; }
    [[nodiscard]] glm::vec3 GetPosition() const { return {modelMatrix[3]}; }

    void SetMaterialOverride(Handle materialHandle, size_t subMeshIndex = 0)
    {
        materialOverrides_[subMeshIndex] = materialHandle;
    }

    [[nodiscard]] Handle GetMaterialHandleOverride(const size_t subMeshIndex) const
    {
        auto it = materialOverrides_.find(subMeshIndex);
        if (it != materialOverrides_.end())
            return it->second;
        return {0, 0};
    }


public:
    glm::mat4 modelMatrix{1.0f};

private:
    Handle modelHandle_{0, 0};
    std::unordered_map<size_t, Handle> materialOverrides_;
};
