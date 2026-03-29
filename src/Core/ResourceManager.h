#pragma once
#include "AssetPool.h"


class Texture;
class Shader;
struct Material;

class ResourceManager
{

public:
    AssetPool<Material> materialPool;
    AssetPool<Shader>   shaderPool;
    AssetPool<Texture>  texturePool;
};
