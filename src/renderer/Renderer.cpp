#include "Renderer.h"

#include <fstream>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderLoader.h"
#include "core/Application.h"
#include "glm/gtc/type_ptr.hpp"
#include "scene/Mesh.h"
#include "scene/Model.h"
#include "scene/GameObject.h"
#include "scene/LightObject.h"

void Renderer::Init()
{
    Log::Trace("Initializing Renderer");
    stbi_set_flip_vertically_on_load(true);

    GLCall(glGenBuffers(1, &UBO_Camera_));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, UBO_Camera_));

    GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::mat4) +sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO_Camera_));

    if constexpr (false)
    {
        // set up basic shader and material
        const char* vertex =
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 uMVP;\n"
            "void main() { gl_Position = uMVP * vec4(aPos, 1.0); }\n";

        const char* frag =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main(){FragColor = vec4(1.0, 0.0, 0.0, 1.0);}\n";
        auto shaderOpt = ShaderLoader::FromSources({.vs = vertex, .fs = frag}, "debug shader");
        if (!shaderOpt)
        {
            Log::Error("Debug Shader creation failed!");
        } else
        {
            Material material{};
            material.Init(rm_->shaderPool.Register("debug", std::move(shaderOpt.value())), *rm_);
            material.SetValue("u_AmbientColor", glm::vec3{1.0f, 0.0f, 0.0f});
            material.SetValue("u_DiffuseColor", glm::vec3{0.0f, 0.0f, 0.0f});
            material.SetValue("u_SpecularColor", glm::vec3{0.0f, 0.0f, 0.0f});
            material.SetValue("u_Shininess", 0.0f);
            debugMaterialHandle = rm_->materialPool.Register("debug", material);
            std::cout << "Registering debug Material and Shader \n";
        }
    }
}

void Renderer::Submit(const RenderCommand& cmd, const RenderPassType passType)
{
    switch (passType)
    {
    case RenderPassType::SHADOW:
        commandQueue_Shadow_.push_back(cmd);
        break;
    case RenderPassType::OPAQUE:
        commandQueue_Opaque_.push_back(cmd);
        break;
    case RenderPassType::TRANSPARENT:
        commandQueue_Transparent_.push_back(cmd);
        break;
    }
}

void Renderer::Submit(const GameObject& obj, const RenderPassType passType, const glm::vec3& camPos, const RasterizationMode rasterMode)
{
    const Handle modelHandle = obj.GetModelHandle();
    Model* model = rm_->modelPool.Get(modelHandle);
    if (!model) return;

    int i = 0;
    for (auto& [meshHandle, materialHandle]: model->GetSubMeshes())
    {
        const Mesh* mesh = rm_->meshPool.Get(meshHandle);

        Handle overrideHandle = obj.GetMaterialHandleOverride(i);
        Handle newMaterialHandle{0, 0};
        if (overrideHandle)
            newMaterialHandle = overrideHandle;
        else
            newMaterialHandle = materialHandle;

        const Material* material  = rm_->materialPool.Get(newMaterialHandle);

        if (!mesh || !material) { ++i; continue; }

        RenderCommand cmd;
        cmd.materialHandle = newMaterialHandle;
        cmd.shaderHandle = material->GetShaderHandle();
        cmd.vertexArrayID = mesh->VAO.GetRendererID();
        cmd.instanceCount = 1;
        cmd.rasterMode = rasterMode;
        cmd.indexCount = mesh->IBO.GetCount();
        cmd.baseVertex = 0;
        cmd.firstIndex = 0;
        cmd.modelMatrix = obj.modelMatrix;

        glm::vec3 objectWorldPos = glm::vec3(obj.modelMatrix[3]);
        const float distanceToCamera = glm::distance(objectWorldPos, camPos);
        cmd.sortKey = GetSortKey(cmd, passType, distanceToCamera);

        Submit(cmd, passType);

        ++i;
    }
}

void Renderer::Execute(const glm::mat4& view, const glm::vec3& viewPos, const glm::mat4& proj, const std::vector<LightObject>& lights)
{
    renderContext_.viewMatrix = view;
    renderContext_.viewPos = viewPos;
    renderContext_.projectionMatrix = proj;

    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, UBO_Camera_));
    GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
    GLCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj)));
    glm::vec4 viewPosPadded{viewPos, 1.0f};
    GLCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::vec4), glm::value_ptr(viewPosPadded)));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    ExecutePass_Shadow(lights);

    SortQueue_Opaque();
    ExecutePass_Opaque(lights);

    SortQueue_Transparent();
    ExecutePass_Transparent(lights);
}

void Renderer::ExecutePass_Shadow(const std::vector<LightObject>& lights)
{
    if (commandQueue_Shadow_.empty())
        return;
}

void Renderer::ExecutePass_Opaque(const std::vector<LightObject>& lights)
{
    if (commandQueue_Opaque_.empty())
        return;

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthMask(GL_TRUE));
    GLCall(glDisable(GL_BLEND));

    Handle activeShaderHandle{0, 0};
    Handle activeMaterialHandle{0, 0};
    unsigned int activeVertexArrayID = 0;
    RasterizationMode activeRasterMode = static_cast<RasterizationMode>(0xFF);

    Shader* shader = nullptr;
    Material* material = nullptr;
    for (const auto& cmd: commandQueue_Opaque_)
    {
        // set raster mode
        if (activeRasterMode != cmd.rasterMode)
        {
            activeRasterMode = cmd.rasterMode;
            switch (activeRasterMode)
            {
            case RasterizationMode::SOLID:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                GLCall(glPointSize(1.0f));
                break;
            case RasterizationMode::WIREFRAME:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                GLCall(glPointSize(1.0f));
                break;
            case RasterizationMode::POINT:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
                GLCall(glPointSize(4.0f));
                break;
            }
        }

        // bind Material
        if (activeMaterialHandle != cmd.materialHandle && cmd.materialHandle)
        {
            activeMaterialHandle = cmd.materialHandle;
            material = rm_->materialPool.Get(activeMaterialHandle);
            if (material)
                material->Bind(*rm_);
        }

        // bind shader and set uniforms
        if (activeShaderHandle != cmd.shaderHandle)
        {
            activeShaderHandle = cmd.shaderHandle;
            shader = rm_->shaderPool.Get(activeShaderHandle);

            if (shader)
            {
                shader->Bind();

                if (material && material->GetLightingEnabled())
                    BindLights(*shader, lights);
                else
                {
                    if (!lights.empty())
                    {
                        shader->SetUniformVec3("uLightColor", lights[0].lightData.diffuse);
                    }
                }
            }
        }

        // model matrix
        if (shader)
            shader->SetUniformMat4f("uModel", cmd.modelMatrix);

        // bind VAO
        if (activeVertexArrayID != cmd.vertexArrayID)
        {
            activeVertexArrayID = cmd.vertexArrayID;
            VertexArray::BindVertexArray(activeVertexArrayID);
        }

        // DRAW CALL
        const void* indexOffset = reinterpret_cast<void*>(cmd.firstIndex * sizeof(unsigned int));
        GLCall(glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES,
            cmd.indexCount,
            GL_UNSIGNED_INT,
            indexOffset,
            cmd.instanceCount,
            cmd.baseVertex
            ));
    }

    commandQueue_Opaque_.clear();
}

void Renderer::ExecutePass_Transparent(const std::vector<LightObject>& lights)
{
    if (commandQueue_Transparent_.empty())
        return;

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthMask(GL_FALSE));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Handle activeShaderHandle{0, 0};
    Handle activeMaterialHandle{0, 0};
    unsigned int activeVertexArrayID = 0;
    RasterizationMode activeRasterMode = static_cast<RasterizationMode>(0xFF);

    Shader* shader = nullptr;
    Material* material = nullptr;
    for (const auto& cmd: commandQueue_Transparent_)
    {
        // set raster mode
        if (activeRasterMode != cmd.rasterMode)
        {
            activeRasterMode = cmd.rasterMode;
            switch (activeRasterMode)
            {
            case RasterizationMode::SOLID:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                GLCall(glPointSize(1.0f));
                break;
            case RasterizationMode::WIREFRAME:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                GLCall(glPointSize(1.0f));
                break;
            case RasterizationMode::POINT:
                GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
                GLCall(glPointSize(4.0f));
                break;
            }
        }

        // bind Material
        if (activeMaterialHandle != cmd.materialHandle && cmd.materialHandle)
        {
            activeMaterialHandle = cmd.materialHandle;
            material = rm_->materialPool.Get(activeMaterialHandle);
            if (material)
                material->Bind(*rm_);
        }

        // bind shader and set uniforms
        if (activeShaderHandle != cmd.shaderHandle)
        {
            activeShaderHandle = cmd.shaderHandle;
            shader = rm_->shaderPool.Get(activeShaderHandle);

            if (shader)
            {
                shader->Bind();

                if (material && material->GetLightingEnabled())
                    BindLights(*shader, lights);
                else
                {
                    if (!lights.empty())
                    {
                        shader->SetUniformVec3("uLightColor", lights[0].lightData.diffuse);
                    }
                }
            }
        }

        // model matrix
        if (shader)
            shader->SetUniformMat4f("uModel", cmd.modelMatrix);

        // bind VAO
        if (activeVertexArrayID != cmd.vertexArrayID)
        {
            activeVertexArrayID = cmd.vertexArrayID;
            VertexArray::BindVertexArray(activeVertexArrayID);
        }

        // DRAW CALL
        const void* indexOffset = reinterpret_cast<void*>(cmd.firstIndex * sizeof(unsigned int));
        GLCall(glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES,
            cmd.indexCount,
            GL_UNSIGNED_INT,
            indexOffset,
            cmd.instanceCount,
            cmd.baseVertex
            ));
    }

    commandQueue_Transparent_.clear();
    glDepthMask(GL_TRUE);
}

void Renderer::BindLights(const Shader& shader, const std::vector<LightObject>& lights) const
{
    if (!lights.empty())
    {
        shader.SetUniformVec3("uLightColor", lights[0].lightData.diffuse);
    }

    for (size_t i = 0; i < lights.size(); ++i)
    {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        shader.SetUniformVec3(prefix + "ambient", lights[i].lightData.ambient);
        shader.SetUniformVec3(prefix + "diffuse", lights[i].lightData.diffuse);
        shader.SetUniformVec3(prefix + "specular", lights[i].lightData.specular);
        shader.SetUniform1f(prefix + "constant", lights[i].lightData.constant);
        shader.SetUniform1f(prefix + "linear", lights[i].lightData.linear);
        shader.SetUniform1f(prefix + "quadratic", lights[i].lightData.quadratic);
        shader.SetUniformVec3(prefix + "position", lights[i].GetPosition());
        shader.SetUniform1i(prefix + "type", static_cast<int>(lights[i].lightData.lightType));
    }
    shader.SetUniform1i("uPointLightCount", static_cast<int>(lights.size()));
}

void Renderer::SortQueue_Opaque()
{
    std::sort(commandQueue_Opaque_.begin(), commandQueue_Opaque_.end(), [](const RenderCommand& a, const RenderCommand& b)
    {
        return a.sortKey < b.sortKey;
    });
}

void Renderer::SortQueue_Transparent()
{
    std::sort(commandQueue_Transparent_.begin(), commandQueue_Transparent_.end(), [](const RenderCommand& a, const RenderCommand& b)
    {
        return a.sortKey < b.sortKey;
    });
}

uint64_t Renderer::GetSortKey(const RenderCommand& cmd, const RenderPassType passType, const float distanceToCamera)
{
    switch (passType)
    {
    case RenderPassType::SHADOW:
        return 0;
    case RenderPassType::OPAQUE:
        return CalculateSortKey_Opaque(cmd.shaderHandle, cmd.materialHandle, distanceToCamera, cmd.rasterMode);
    case RenderPassType::TRANSPARENT:
        return CalculateSortKey_Transparent(cmd.shaderHandle, cmd.materialHandle, distanceToCamera, cmd.rasterMode);
    }
    return 0;
}

uint64_t Renderer::CalculateSortKey_Opaque(const Handle shaderHandle, const Handle materialHandle, const float distanceToCamera, RasterizationMode mode)
{
    const uint64_t wireMode = static_cast<uint64_t>(mode);
    const uint64_t shaderID = static_cast<uint64_t>(shaderHandle.id);
    const uint64_t materialID = static_cast<uint64_t>(materialHandle.id);
    const uint64_t depth = static_cast<uint64_t>(GetQuantizedDepth_20Bit(distanceToCamera));

    // Bit 62-63 (2 Bit) mode
    // Bit 40-61 (22 Bit) shaderID
    // BIT 20-39 (20 Bit) materialID
    // BIT 0-19 (20 Bit) depth
    return (wireMode << 62) | (shaderID << 40) | (materialID << 20) | (depth << 0);
}

uint64_t Renderer::CalculateSortKey_Transparent(const Handle shaderHandle, const Handle materialHandle, const float distanceToCamera, RasterizationMode mode)
{
    const uint64_t wireMode = static_cast<uint64_t>(mode);
    const uint64_t depth = static_cast<uint64_t>(GetQuantizedDepth_20Bit(distanceToCamera));
    const uint64_t shaderID = static_cast<uint64_t>(shaderHandle.id);
    const uint64_t materialID = static_cast<uint64_t>(materialHandle.id);

    // Bit 62-63 (2 Bit) mode
    // Bit 40-61 (20 Bit) depth
    // BIT 20-39 (22 Bit) shaderID
    // BIT 0-19 (20 Bit) materialID
    return (wireMode << 62) | (depth << 42) | (shaderID << 20) | (materialID << 0);
}

uint32_t Renderer::GetQuantizedDepth_20Bit(const float distanceToCamera)
{
    constexpr float MAX_DEPTH = 1000.0f;
    const float clamped = std::clamp(distanceToCamera, 0.0f, MAX_DEPTH);
    const float normalized = clamped / MAX_DEPTH;
    constexpr float MAX_UINT20 = static_cast<float>((1 << 20) - 1);
    return static_cast<uint32_t>(normalized * MAX_UINT20);
}

void Renderer::Clear()
{
    commandQueue_Opaque_.clear();
    commandQueue_Shadow_.clear();
    commandQueue_Transparent_.clear();
    GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::ImGui_BeginFrame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::ImGui_EndFrame() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_);
}