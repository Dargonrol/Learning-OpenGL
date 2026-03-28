#include "LightSource.h"

#include "../../Core/Material.h"

LightSource::LightSource()
{
    Shader shader();

    material = std::make_shared<Material>();
    material->ambient = {0.2f, 0.2f, 0.2f};
    material->diffuse = {0.5f, 0.5f, 0.5f};
    material->specular = {1.0f, 1.0f, 1.0f};
    material->shininess = 0.0f;
}

// shader pool
// Material pool
// getShader("shader1") returns weakptr?  or returns handle
// getMaterial("metal") returns weakptr? or returns handle
// handle so that it's not tied to memory and can be hotswapped.