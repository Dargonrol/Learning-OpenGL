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
#include "glm/gtc/type_ptr.hpp"
#include "scene/Mesh.h"
#include "scene/Model.h"
#include "scene/GameObject.h"
#include "scene/Camera.h"
#include "scene/LightObject.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << ") in " << function << " at " << file << ":" << line << "\n";
        return false;
    }
    return true;
}

Renderer::Renderer(ResourceManager& resource_manager, GLFWwindow* window, Core::Application& APP) : app(APP), m_window(window), rm_(&resource_manager){}

void Renderer::Init()
{
    stbi_set_flip_vertically_on_load(true);

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
    material.shaderHandle = rm_->shaderPool.Register("debug", std::move(shader));
    debugShaderHandle = material.shaderHandle;
    material.ambient = {1.0f, 0.0f, 0.0f};
    material.diffuse = {0.0f, 0.0f, 0.0f};
    material.shininess = 0.0f;
    material.specular = {0.0f, 0.0f, 0.0f};
    debugMaterialHandle = rm_->materialPool.Register("debug", material);
    std::cout << "Registering debug Material and Shader \n";
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, Shader &shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray &va, Shader &shader, unsigned int count) const
{
    shader.Bind();
    va.Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
}

void Renderer::Draw(const Mesh& mesh) const
{
    mesh.VAO.Bind();
    mesh.IBO.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, mesh.IBO.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const GameObject& gameObj, Camera& cam, const std::vector<LightObject>& lights) const
{
    Handle modelHandle = gameObj.GetModelHandle();
    Model* model = rm_->modelPool.Get(modelHandle);
    if (!model) return;

    int i = 0;
    for (const auto& [meshHandle, materialHandle]: model->GetSubMeshes())
    {
        const Mesh* mesh          = rm_->meshPool.Get(meshHandle);
        const Material* material  = rm_->materialPool.Get(materialHandle);

        Handle overrideHandle = gameObj.GetMaterialHandleOverride(i);
        if (overrideHandle)
            material = rm_->materialPool.Get(overrideHandle);

        Shader* shader      = rm_->shaderPool.Get(material->shaderHandle);

        if (!mesh || !material || !shader) { ++i; continue; }

        shader->Bind();
        mesh->VAO.Bind();
        mesh->IBO.Bind();
        mesh->VBO.Bind();

        if (material->diffuseMap)
        {
            const Texture* diffTex = rm_->texturePool.Get(material->diffuseMap);
            diffTex->Bind(0);
            shader->SetUniform1i("material.diffuse", 0);
        }
        if (material->specularMap)
        {
            const Texture* specTex = rm_->texturePool.Get(material->specularMap);
            specTex->Bind(1);
            shader->SetUniform1i("material.specular", 1);
        }

        shader->SetUniformMat4f("uView", cam.GetViewMatrix());
        shader->SetUniformMat4f("uProj", cam.GetProjectionMatrix());
        shader->SetUniformMat4f("uModel", gameObj.modelMatrix);
        shader->SetUniformVec3("uViewPos", cam.GetPosition());

        shader->SetUniformVec3("material.diffuseColor", material->diffuse);
        shader->SetUniform1f("material.shininess", material->shininess);

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

        Draw(*mesh);
        ++i;
    }
}

void Renderer::DrawLight(LightObject& obj, Camera& cam) const
{
    Handle modelHandle = obj.GetModelHandle();
    Model* model = rm_->modelPool.Get(modelHandle);
    if (!model) return;

    int i = 0;
    for (const auto& [meshHandle, materialHandle]: model->GetSubMeshes())
    {
        const Mesh* mesh          = rm_->meshPool.Get(meshHandle);
        const Material* material  = rm_->materialPool.Get(materialHandle);

        Handle overrideHandle{};
        if (overrideHandle = obj.GetMaterialHandleOverride(i))
            material = rm_->materialPool.Get(overrideHandle);

        Shader* shader      = rm_->shaderPool.Get(material->shaderHandle);

        if (!mesh || !material || !shader) { ++i; continue; }

        shader->Bind();
        mesh->VAO.Bind();
        mesh->IBO.Bind();
        mesh->VBO.Bind();

        shader->SetUniformMat4f("uView", cam.GetViewMatrix());
        shader->SetUniformMat4f("uProj", cam.GetProjectionMatrix());
        shader->SetUniformMat4f("uModel", obj.modelMatrix);
        shader->SetUniformVec3("uViewPos", cam.GetPosition());
        shader->SetUniformVec3("uLightColor", obj.lightData.diffuse);


        Draw(*mesh);
        ++i;
    }
}

void Renderer::WireDraw(const GameObject& gameObj, const glm::mat4& MVP) const
{
    if (rm_->shaderPool.Exists(debugShaderHandle))
    {
        rm_->shaderPool.Get(debugShaderHandle)->Bind();
        rm_->shaderPool.Get(debugShaderHandle)->SetUniformMat4f("uMVP", MVP);
    }

    Model* model = rm_->modelPool.Get(gameObj.GetModelHandle());
    if (!model)
        return;

    for (const auto& [meshHandle, materialHandle]: model->GetSubMeshes())
    {
        const Mesh* mesh = rm_->meshPool.Get(meshHandle);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Draw(*mesh);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (rm_->shaderPool.Exists(debugShaderHandle))
        rm_->shaderPool.Get(debugShaderHandle)->Unbind();
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

Renderer::~Renderer() = default;
