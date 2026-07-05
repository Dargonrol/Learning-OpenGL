#pragma once
#include "AssetPool.h"
#include "Render/Material.h"
#include "Render/Texture.h"
#include "Render/Shader.h"

class ResourceManager
{
public:
    AssetPool<Material> materialPool;
    AssetPool<Shader>   shaderPool;
    AssetPool<Texture>  texturePool;
};
