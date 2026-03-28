#pragma once
#include <memory>

// fwd declares
struct Material;
// fwd declares

class Object
{
public:
    virtual ~Object() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

public:
    std::shared_ptr<Material> material = nullptr; // maybe a handle or weakptr in the actual engine?
};