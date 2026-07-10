#pragma once
#include "GameObject.h"
#include "LightData.h"

class LightObject : public GameObject
{
public:
    using GameObject::GameObject;


public:
    LightData lightData;

};
