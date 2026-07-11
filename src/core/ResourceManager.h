#pragma once
#include "AssetPool.h"

#include "renderer/Material.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/Mesh.h"
#include "scene/Model.h"


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
