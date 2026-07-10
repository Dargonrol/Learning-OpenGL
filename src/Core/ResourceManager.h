#pragma once
#include "AssetPool.h"

struct Texture;
class Shader;
struct Material;
struct Mesh;


class ResourceManager
{
public:
    AssetPool<Material> materialPool;
    AssetPool<Shader>   shaderPool;
    AssetPool<Texture>  texturePool;
    AssetPool<Mesh>     meshPool;
};
