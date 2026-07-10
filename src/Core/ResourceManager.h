#pragma once
#include "AssetPool.h"

#include "Core/Render/Material.h"
#include "Core/Render/Shader.h"
#include "Core/Render/Texture.h"
#include "Core/Render/Mesh.h"


class ResourceManager
{
public:
    static ResourceManager& Get()
    {
        static ResourceManager instance;
        return instance;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;


public:
    AssetPool<Material> materialPool{};
    AssetPool<Shader>   shaderPool{};
    AssetPool<Texture>  texturePool{};
    AssetPool<Mesh>     meshPool{};


private:
    ResourceManager() = default;
    ~ResourceManager() = default;
};
