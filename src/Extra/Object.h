#pragma once
#include "../Core/AssetPool.h"

class Object
{
public:
    virtual ~Object() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

public:
    Handle materialHandle;
    Handle textureHandle;
};