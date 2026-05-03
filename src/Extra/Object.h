#pragma once
#include "Light.h"
#include "../Core/AssetPool.h"
#include "../Core/ResourceManager.h"
#include "glm/matrix.hpp"

class Object
{
public:
    virtual ~Object() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void BindAll(ResourceManager& rm) = 0;
    virtual size_t GetVerticesCount() = 0;
    virtual Shader* GetShader(ResourceManager& rm)
    {
        Material* material = rm.materialPool.Get(materialHandle);
        if (material != nullptr)
            return rm.shaderPool.Get(material->shaderHandle);
        return nullptr;
    }
    virtual Material* GetMaterial(ResourceManager& rm)
    {
        return rm.materialPool.Get(materialHandle);
    }

    void SetLightUniforms(ResourceManager& rm, Object& obj, const size_t lightNum, Light::LightType type)
    {
        const auto& shader = obj.GetShader(rm);
        if (shader == nullptr)
            return;
        shader->Bind();
        std::string prefix = "lights[" + std::to_string(lightNum) + "].";
        shader->SetUniformVec3(prefix + "ambient", light.ambient);
        shader->SetUniformVec3(prefix + "diffuse", light.diffuse);
        shader->SetUniformVec3(prefix + "specular", light.specular);
        shader->SetUniform1f(prefix + "constant", light.constant);
        shader->SetUniform1f(prefix + "linear", light.linear);
        shader->SetUniform1f(prefix + "quadratic", light.quadratic);
        shader->SetUniformVec3(prefix + "position", glm::vec3(modelMatrix[3]));
        shader->SetUniform1i(prefix + "type", static_cast<int>(type));
    }

public:
    Handle materialHandle;
    Handle textureHandle;

    glm::mat4 modelMatrix{1.0f};

    bool lightSource = false;
    Light light;


};
