#pragma once
#include "../Object.h"

class LightSource : Object
{
    LightSource();

    void Update(float deltaTime) override;
    void Render() override;
};
