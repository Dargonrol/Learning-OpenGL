#pragma once
#include "../Core/AssetPool.h"
#include "../Core/ResourceManager.h"

class Object
{
public:
    virtual ~Object() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void BindAll(ResourceManager& rm) = 0;
    virtual size_t GetVerticesCount() = 0;
    virtual Shader* GetShader(ResourceManager& rm)
    {
        Material* material = rm.materialPool.Get(materialHandle);
        if (material != nullptr)
            return rm.shaderPool.Get(material->shaderHandle);
        return nullptr;
    }
    virtual Material* GetMaterial(ResourceManager& rm)
    {
        return rm.materialPool.Get(materialHandle);
    }

public:
    Handle materialHandle;
    Handle textureHandle;
};
