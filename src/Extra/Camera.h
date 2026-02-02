#pragma once
#include <string>

#include "glm/gtc/type_ptr.hpp"

class GLFWwindow;

enum class CameraMode
{
    FPS = 0,
    ORBIT = 1
};

/**
 * Orbit Mode:  Camera always looks at target, Yaw Roll and Pitch is ignored
 * FPS Mode:    Target get's ignored
 */
class Camera
{
public:
    Camera();
    ~Camera();

    void Update();
    void SetTarget(const glm::vec3 &target);

    void SetPosition(const glm::vec3& pos);
    void AddPosition(const glm::vec3& pos);

    void SetYaw(float yaw);
    void SetPitch(float pitch);
    void SetRoll(float roll);
    void AddYaw(float yaw);
    void AddPitch(float pitch);
    void AddRoll(float roll);

    void Reset();
    void SetMode(CameraMode mode);

    [[nodiscard]] const glm::mat4& GetViewMatrix() const;
    [[nodiscard]] const glm::vec3& GetDirectionVector() const;
    [[nodiscard]] const glm::vec3& GetUpVector() const;
    [[nodiscard]] const glm::vec3& GetTargetPos() const;
    [[nodiscard]] float GetTargetDistance() const;
    [[nodiscard]] CameraMode GetMode() const;

    static const std::array<const char*, 2>& GetCameraModes()
    {
        static std::array<const char*, 2> names = {"FPS", "ORBIT"};
        return names;
    }

    static const char* GetCameraMode(const CameraMode mode)
    {
        switch (mode)
        {
            case CameraMode::FPS: return "FPS";
            case CameraMode::ORBIT: return "Orbit";
        }
        return "INVALID";
    }

    void HandleGenericCameraControls(GLFWwindow* window, float deltaTime, float camSpeed = 10.0f, float camSensitivity = 4.0f);

private:
    void UpdateVectors();
    void Init();

private:
    glm::mat4 m_view{};
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
};