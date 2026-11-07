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

enum class RenderPassType : uint8_t
{
    SHADOW= 0,
    OPAQUE = 1,
    TRANSPARENT = 2
};

struct RenderCommand
{
    uint64_t sortKey = 0;

    unsigned int vertexArrayID = 0;
    Handle shaderHandle{0,0};
    Handle materialHandle{0, 0};

    glm::mat4 modelMatrix{1.0f};
    float distanceFromCamera = 0.0f;

    uint32_t indexCount = 0;
    uint32_t firstIndex = 0;
    int32_t baseVertex = 0;
    uint32_t instanceCount = 1;

    std::string name = "gr";

    RasterizationMode rasterMode = RasterizationMode::SOLID;
};

class Renderer
{
public:
    explicit Renderer(ResourceManager& resource_manager, GLFWwindow* window, Core::Application& APP) : app(APP), m_window(window), rm_(&resource_manager) {}
    ~Renderer() = default;

    void Init();

    void Submit(const RenderCommand& cmd, RenderPassType passType);
    void Submit(const GameObject& obj, RenderPassType passType, const glm::vec3& camPos, RasterizationMode rasterMode = RasterizationMode::SOLID);
    void Execute(const glm::mat4& view, const glm::vec3& viewPos, const glm::mat4& proj, const std::vector<LightObject>& lights);

    void Clear();

    [[nodiscard]] GLFWwindow& GetWindow() const { return *m_window; }

    void ImGui_BeginFrame() const;
    void ImGui_EndFrame() const;


public:
    Core::Application& app;

private:
    void ExecutePass_Shadow(const std::vector<LightObject>& lights);
    void ExecutePass_Opaque(const std::vector<LightObject>& lights);
    void ExecutePass_Transparent(const std::vector<LightObject>& lights);

    void SortQueue_Opaque();
    void SortQueue_Transparent();

    static uint64_t CalculateSortKey_Opaque(Handle shaderHandle, Handle materialHandle, float distanceToCamera, RasterizationMode mode = RasterizationMode::SOLID);
    static uint64_t CalculateSortKey_Transparent(Handle shaderHandle, Handle materialHandle, float distanceToCamera, RasterizationMode mode = RasterizationMode::SOLID);

    [[nodiscard]] static uint64_t GetSortKey(const RenderCommand& cmd, RenderPassType passType, float distanceToCamera);
    [[nodiscard]] static uint32_t GetQuantizedDepth_20Bit(float distanceToCamera);

    void BindLights(const Shader& shader, const std::vector<LightObject>& lights) const;

private:
    GLFWwindow* m_window = nullptr;
    ResourceManager* rm_ = nullptr;

    std::vector<RenderCommand> commandQueue_Opaque_;
    std::vector<RenderCommand> commandQueue_Transparent_;
    std::vector<RenderCommand> commandQueue_Shadow_;
    RasterizationMode currentRasterMode_ = RasterizationMode::SOLID;

    Handle debugMaterialHandle = {0,0};
    Handle debugShaderHandle = {0,0};

    struct RenderContext
    {
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 viewPos;
    } renderContext_{};
};