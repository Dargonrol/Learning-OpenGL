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
#include "core/Application.h"
#include "glm/gtc/type_ptr.hpp"
#include "scene/Mesh.h"
#include "scene/Model.h"
#include "scene/GameObject.h"
#include "scene/LightObject.h"

Renderer::Renderer(ResourceManager& resource_manager, GLFWwindow* window, Core::Application& APP) : app(APP), m_window(window), rm_(&resource_manager){}

void Renderer::Init()
{
    stbi_set_flip_vertically_on_load(true);

    if (false)
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
        int error = 0;
        auto shader = Shader(vertex, frag, error);
        if (error)
        {
            std::cerr << "Failed Debug Shader creation!" << std::endl;
            return;
        }
        Material material{};
        material.Init(rm_->shaderPool.Register("debug", std::move(shader)), *rm_);
        material.SetValue("u_AmbientColor", glm::vec3{1.0f, 0.0f, 0.0f});
        material.SetValue("u_DiffuseColor", glm::vec3{0.0f, 0.0f, 0.0f});
        material.SetValue("u_SpecularColor", glm::vec3{0.0f, 0.0f, 0.0f});
        material.SetValue("u_Shininess", 0.0f);
        debugMaterialHandle = rm_->materialPool.Register("debug", material);
        std::cout << "Registering debug Material and Shader \n";
    }
}

void Renderer::Submit(const RenderCommand& cmd)
{
    commandQueue_.push_back(cmd);
}

void Renderer::SortQueue()
{
    std::sort(commandQueue_.begin(), commandQueue_.end(), [](const RenderCommand& a, const RenderCommand& b)
    {
        return a.sortKey < b.sortKey;
    });
}

// TODO: add different render passes.
void Renderer::Execute(const glm::mat4& view, const glm::vec3& viewPos, const glm::mat4& proj, std::vector<LightObject> lights)
{
    SortQueue();
    Handle activeShaderHandle = {0, 0};
    Handle activeMaterialHandle = {0, 0};
    unsigned int vertexArrayID = 0;

    for (const auto& cmd: commandQueue_)
    {
        if (activeShaderHandle != cmd.shaderHandle)
        {
            activeShaderHandle = cmd.shaderHandle;

            Shader* shader = app.GetResourceManager()->shaderPool.Get(activeShaderHandle);
            shader->Bind();

            shader->SetUniformMat4f("uView", view);
            shader->SetUniformMat4f("uProj", proj);
            shader->SetUniformVec3("uViewPos", viewPos);

            if (!lights.empty())
            {
                shader->SetUniformVec3("uLightColor", lights[0].lightData.diffuse);
            }

            for (size_t i = 0; i < lights.size(); ++i)
            {
                std::string prefix = "lights[" + std::to_string(i) + "].";
                shader->SetUniformVec3(prefix + "ambient", lights[i].lightData.ambient);
                shader->SetUniformVec3(prefix + "diffuse", lights[i].lightData.diffuse);
                shader->SetUniformVec3(prefix + "specular", lights[i].lightData.specular);
                shader->SetUniform1f(prefix + "constant", lights[i].lightData.constant);
                shader->SetUniform1f(prefix + "linear", lights[i].lightData.linear);
                shader->SetUniform1f(prefix + "quadratic", lights[i].lightData.quadratic);
                shader->SetUniformVec3(prefix + "position", lights[i].GetPosition());
                shader->SetUniform1i(prefix + "type", static_cast<int>(lights[i].lightData.lightType));
            }
            shader->SetUniform1i("uPointLightCount", static_cast<int>(lights.size()));
        }

        if (vertexArrayID != cmd.vertexArrayID)
        {
            vertexArrayID = cmd.vertexArrayID;
            VertexArray::BindVertexArray(vertexArrayID);
        }

        Shader* currentShader = app.GetResourceManager()->shaderPool.Get(activeShaderHandle);

        currentShader->SetUniformMat4f("uModel", cmd.modelMatrix);

        if (activeMaterialHandle != cmd.materialHandle && cmd.materialHandle)
        {
            activeMaterialHandle = cmd.materialHandle;

            Material* material = app.GetResourceManager()->materialPool.Get(activeMaterialHandle);
            if (material)
                material->Bind(*rm_);
        }

        if (currentRasterMode_ != cmd.rasterMode)
        {
            switch (cmd.rasterMode)
            {
            case RasterizationMode::SOLID:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPointSize(1.0f);
                currentRasterMode_ = RasterizationMode::SOLID;
                break;
            case RasterizationMode::WIREFRAME:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glPointSize(1.0f);
                currentRasterMode_ = RasterizationMode::WIREFRAME;
                break;
            case RasterizationMode::POINT:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                glPointSize(4.0f);
                currentRasterMode_ = RasterizationMode::POINT;
                break;
            }
        }

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

    commandQueue_.clear();
}

void Renderer::Clear() const
{
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
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

    glfwSwapBuffers(m_window);
}

uint64_t Renderer::CalculateSortKey(const Handle shaderHandle, const Handle materialHandle, RasterizationMode mode)
{
    uint64_t shaderID = static_cast<uint64_t>(shaderHandle.id);
    uint64_t materialID = static_cast<uint64_t>(materialHandle.id);
    uint64_t wireMode = static_cast<uint64_t>(mode);

    // Bit 62-63 (2 Bit) mode
    // Bit 32-61 (30 Bit) shaderID
    // BIT 0-31 (32 Bit) materialID
    return (wireMode << 62) | (shaderID << 32) | materialID;
}

Renderer::~Renderer() = default;

void Renderer::BuildCommands(const GameObject& obj, const RasterizationMode mode, ResourceManager& rm, std::vector<RenderCommand>& commandQueue)
{
    const Handle modelHandle = obj.GetModelHandle();
    Model* model = rm.modelPool.Get(modelHandle);
    if (!model) return;

    int i = 0;
    for (auto& [meshHandle, materialHandle]: model->GetSubMeshes())
    {
        const Mesh* mesh = rm.meshPool.Get(meshHandle);

        Handle overrideHandle = obj.GetMaterialHandleOverride(i);
        Handle newMaterialHandle{0, 0};
        if (overrideHandle)
            newMaterialHandle = overrideHandle;
        else
            newMaterialHandle = materialHandle;

        const Material* material  = rm.materialPool.Get(newMaterialHandle);

        if (!mesh || !material) { ++i; continue; }

        RenderCommand cmd;
        cmd.materialHandle = newMaterialHandle;
        cmd.shaderHandle = material->GetShaderHandle();
        cmd.vertexArrayID = mesh->VAO.GetRendererID();
        cmd.instanceCount = 1;
        cmd.rasterMode = mode;
        cmd.indexCount = mesh->IBO.GetCount();
        cmd.baseVertex = 0;
        cmd.firstIndex = 0;
        cmd.modelMatrix = obj.modelMatrix;
        cmd.sortKey = CalculateSortKey(cmd.shaderHandle, cmd.materialHandle, mode);

        commandQueue.push_back(cmd);

        ++i;
    }
}

void Renderer::BuildCommands(const GameObject& obj, RasterizationMode mode, ResourceManager& rm)
{
    BuildCommands(obj, mode, rm, commandQueue_);
}
