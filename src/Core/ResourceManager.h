#pragma once
#include "AssetPool.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

class ResourceManager
{
public:
    AssetPool<Material> materialPool;
    AssetPool<Shader>   shaderPool;
    AssetPool<Texture>  texturePool;
};
