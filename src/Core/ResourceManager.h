#pragma once
#include "AssetPool.h"

#include "Core/Render/Material.h"
#include "Core/Render/Shader.h"
#include "Core/Render/Texture.h"
#include "Core/Render/Mesh.h"
#include "Core/Render/Model.h"


class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;


public:
    AssetPool<Material> materialPool{};
    AssetPool<Shader>   shaderPool{};
    AssetPool<Texture>  texturePool{};
    AssetPool<Mesh>     meshPool{};
    AssetPool<Model>    modelPool{};
};
