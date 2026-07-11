#pragma once
#include "scene/GameObject.h"
#include "scene/LightData.h"

class LightObject : public GameObject
{
public:
    using GameObject::GameObject;


public:
    LightData lightData;

};
