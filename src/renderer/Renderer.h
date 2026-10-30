#pragma once

#include "VertexArray.h"
#include "core/AssetPool.h"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp"

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
class IndexBuffer;
class Shader;
class GLFWwindow;
class VertexArray;

enum class RasterizationMode : uint8_t
{
    SOLID = 0,
    WIREFRAME = 1,
    POINT = 2
};

struct RenderCommand
{
    uint64_t sortKey = 0;

    unsigned int vertexArrayID = 0;
    Handle shaderHandle{0,0};
    Handle materialHandle{0, 0};

    glm::mat4 modelMatrix{1.0f};

    uint32_t indexCount = 0;
    uint32_t firstIndex = 0;
    int32_t baseVertex = 0;
    uint32_t instanceCount = 1;

    RasterizationMode rasterMode = RasterizationMode::SOLID;
};

class Renderer
{
public:
    explicit Renderer(ResourceManager& resource_manager, GLFWwindow* window, Core::Application& APP);
    ~Renderer();

    void Init();

    void Submit(const RenderCommand& cmd);
    void Execute(const glm::mat4& view, const glm::vec3& viewPos, const glm::mat4& proj, std::vector<LightObject> lights);

    void Clear() const;

    [[nodiscard]] GLFWwindow& GetWindow() const { return *m_window; }
    [[nodiscard]] std::vector<RenderCommand>& GetRenderQueue() { return commandQueue_; }

    void ImGui_BeginFrame() const;
    void ImGui_EndFrame() const;

    static uint64_t CalculateSortKey(Handle shaderHandle, Handle materialHandle, RasterizationMode mode = RasterizationMode::SOLID);
    static void BuildCommands(const GameObject& obj, RasterizationMode mode, ResourceManager& rm, std::vector<RenderCommand>& commandQueue);
    void BuildCommands(const GameObject& obj, RasterizationMode mode, ResourceManager& rm);

public:
    Core::Application& app;

private:
    void SortQueue();

private:
    GLFWwindow* m_window = nullptr;
    ResourceManager* rm_ = nullptr;

    std::vector<RenderCommand> commandQueue_;
    RasterizationMode currentRasterMode_ = RasterizationMode::SOLID;

    Handle debugMaterialHandle = {0,0};
    Handle debugShaderHandle = {0,0};
};