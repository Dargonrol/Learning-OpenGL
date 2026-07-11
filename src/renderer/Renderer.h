#pragma once

#include <string>
#include "core/AssetPool.h"
#include "glm/fwd.hpp"

namespace Core
{
    class Application;
}

class LightObject;
class Cube;
class GameObject;
class Model;
class Camera;
struct Mesh;
class ResourceManager;
class Object;
class VertexArray;
class IndexBuffer;
class Shader;
class GLFWwindow;

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    explicit Renderer(ResourceManager& resource_manager, GLFWwindow* window, Core::Application& APP);
    ~Renderer();

    void Init();

    void Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const;
    void Draw(const VertexArray& va, Shader& shader, unsigned int count) const;

    void Draw(const GameObject& gameObj, Camera& cam, const std::vector<LightObject>& lights) const;
    void DrawLight(LightObject& obj, Camera& cam) const;
    void WireDraw(const GameObject& gameObj, const glm::mat4& MVP) const;

    void Clear() const;

    [[nodiscard]] GLFWwindow& GetWindow() const { return *m_window; }

    void ImGui_BeginFrame() const;
    void ImGui_EndFrame() const;

public:
    Core::Application& app;

private:
    void Draw(const Mesh& mesh) const;

private:
    GLFWwindow* m_window = nullptr;
    ResourceManager* rm_ = nullptr;

    Handle debugMaterialHandle = {0,0};
    Handle debugShaderHandle = {0,0};
};