#pragma once

#include "glm/gtc/type_ptr.hpp"

class GLFWwindow;

enum class CameraMode
{
    FPS = 0,
    ORBIT = 1,
    ORTHO = 2,
    THIRD
};

struct OrthoData
{
    struct OrthoBounds
    {
        float left = 0.0f;
        float right = 100.0f;
        float bottom = 0.0f;
        float top = 100.0f;
    };

    OrthoBounds bounds;
    float near = 1.0f;
    float far = -1.0f;
    float zoom = 1.0f;
};

struct PerspectiveData
{
    float fov = 45;
    float aspect = 300.0f/200.0f;
    float near = 0.1f;
    float far = 100.0f;
};

/**
 * Orbit Mode:  Camera always looks at target, Yaw Roll and Pitch is ignored
 * FPS Mode:    Target get's ignored
 */
class Camera
{
public:
    explicit Camera(CameraMode mode = CameraMode::FPS);
    ~Camera();

    void Update();
    void SetTarget(const glm::vec3 &target);
    void SyncMouse(double x, double y);

    void SetPosition(const glm::vec3& pos);
    void AddPosition(const glm::vec3& pos);

    void SetYaw(float yaw);
    void SetPitch(float pitch);
    void SetRoll(float roll);
    void AddYaw(float yaw);
    void AddPitch(float pitch);
    void AddRoll(float roll);

    // projection
    void SetFOV(float fov);
    void SetNear(float near);
    void SetFar(float far);
    void SetAspectRatio(float ratio);
    void SetOrthoZoom(float zoom);
    void SetOrthoBounds(OrthoData::OrthoBounds bounds);
    void SetOrthoBounds(float left, float right, float bottom, float top);
    void SetOrthoData(const OrthoData &ortho_data);
    void SetPerspectiveData(const PerspectiveData& perspective_data);
    void SetProjectionMatrix(const glm::mat4& proj);

    [[nodiscard]] float GetFOV() const;
    [[nodiscard]] float GetNear() const;
    [[nodiscard]] float GetFar() const;
    [[nodiscard]] float GetAspectRatio() const;
    [[nodiscard]] float GetOrthoZoom() const;
    [[nodiscard]] OrthoData::OrthoBounds& GetOrthoBounds();
    [[nodiscard]] OrthoData& GetOrthoData();
    [[nodiscard]] PerspectiveData& GetPerspectiveData();
    [[nodiscard]] glm::mat4& GetProjectionMatrix();

    void Reset();
    void SetMode(CameraMode mode);
    void SetMouseSensitivity(float value);

    [[nodiscard]] const glm::mat4& GetViewMatrix() const;
    [[nodiscard]] const glm::vec3& GetDirectionVector() const;
    [[nodiscard]] const glm::vec3& GetUpVector() const;
    [[nodiscard]] const glm::vec3& GetTargetPos() const;
    [[nodiscard]] float GetTargetDistance() const;
    [[nodiscard]] CameraMode GetMode() const;
    [[nodiscard]] float GetMouseSensitivity() const;

    static const std::array<const char*, 4>& GetCameraModes()
    {
        static std::array<const char*, 4> names = {"FPS", "ORBIT", "ORTHO", "THIRD" };
        return names;
    }

    static const char* GetCameraMode(const CameraMode mode)
    {
        switch (mode)
        {
            case CameraMode::FPS: return "FPS";
            case CameraMode::ORBIT: return "Orbit";
            case CameraMode::ORTHO: return "Ortho";
            case CameraMode::THIRD: return "Third";
        }
        return "INVALID";
    }

    void HandleGenericCameraControls(GLFWwindow* window, float deltaTime, float camSpeed = 10.0f, float camSensitivity = 4.0f);

public:
    bool enableMouseControl = false;

private:
    void Init();
    void HandleGenericCameraControlsFPS(GLFWwindow* window, float deltaTime, float camSpeed = 10.0f, float camSensitivity = 4.0f);
    void HandleGenericCameraControlsOrtho(GLFWwindow* window, float deltaTime, float camSpeed);

private:
    glm::mat4 m_view{};
    glm::mat4 m_proj{};
    CameraMode m_mode = CameraMode::FPS;

    glm::vec3 m_target{0.0f, 0.0f, 0.0f,};
    glm::vec3 m_pos{0.0f, 0.0f, 3.0f};
    glm::vec3 m_worldUp{0.0f, 1.0f, 0.0f};
    glm::vec3 m_direction{};
    glm::vec3 m_right{};
    glm::vec3 m_up{};

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_roll = 0.0f;

    OrthoData m_orthoData;
    PerspectiveData m_perspectiveData;

    float m_mouseSensitivity = 0.002f;
    double m_mouseLastX = 0;
    double m_mouseLastY = 0;
    double m_mouseX = 0;
    double m_mouseY = 0;
};