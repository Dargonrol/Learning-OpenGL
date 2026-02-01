#include "Camera.h"

Camera::Camera()
{
    Init();
}

void Camera::Init()
{
    m_mode = CameraMode::FPS;
    m_target = {0.0f, 0.0f, 0.0f};
    m_pos = {0.0f, 0.0f, 3.0f};
    m_worldUp = {0.0f, 1.0f, 0.0f};
    m_direction = glm::normalize(m_target - m_pos);
    m_right = glm::normalize(glm::cross(m_direction, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_direction));
    m_yaw = -glm::half_pi<float>();
    m_pitch = 0.0f;
    m_roll = 0.0f;

    m_view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),    // pos vector
        glm::vec3(0.0f, 0.0f, 0.0f),    // target vector
        glm::vec3(0.0f, 1.0f, 0.0f)     // world up vector
        );
}

Camera::~Camera() = default;

void Camera::Update()
{
    UpdateVectors();
}

void Camera::SetPosition(const glm::vec3 &pos) { m_pos = pos; }
void Camera::AddPosition(const glm::vec3 &pos) { m_pos += pos; }
void Camera::SetYaw(float yaw) { m_yaw = yaw; }
void Camera::SetPitch(float pitch) { m_pitch = pitch; }
void Camera::SetRoll(float roll) { m_roll = roll; }
void Camera::AddYaw(float yaw) { m_yaw += yaw; }
void Camera::AddPitch(float pitch) { m_pitch += pitch; }
void Camera::AddRoll(float roll) { m_roll += roll; }

const glm::vec3 & Camera::GetTargetPos() const { return m_target; }
float Camera::GetTargetDistance() const { return glm::length(m_pos - m_target); }
CameraMode Camera::GetMode() const { return m_mode; }
const glm::mat4& Camera::GetViewMatrix() const { return m_view; }
const glm::vec3 &Camera::GetDirectionVector() const { return m_direction; }
const glm::vec3 & Camera::GetUpVector() const { return m_up; }

void Camera::SetTarget(const glm::vec3 &target) { m_target = target; }

void Camera::UpdateVectors()
{
    switch (m_mode)
    {
        case CameraMode::FPS:
            m_pitch = glm::clamp(m_pitch, -1.55f, 1.55f);
            m_direction.x = glm::cos(m_pitch) * glm::cos(m_yaw);
            m_direction.y = glm::sin(m_pitch);
            m_direction.z = glm::cos(m_pitch) * glm::sin(m_yaw);
            m_direction = glm::normalize(m_direction);
            m_right = glm::normalize(glm::cross(m_direction, m_worldUp));
            m_up = glm::normalize(glm::cross(m_right, m_direction));

            if (m_roll != 0.0f)
            {
                glm::mat4 rollRot = glm::rotate(glm::mat4(1.0f), m_roll, m_direction);
                m_right = glm::normalize(glm::vec3(rollRot * glm::vec4(m_right, 0.0f)));
                m_up = glm::normalize(glm::vec3(rollRot * glm::vec4(m_up, 0.0f)));
            }
            m_view = glm::lookAt(m_pos, m_pos + m_direction, m_up);
            break;
        case CameraMode::ORBIT:
            m_direction = glm::normalize(m_target - m_pos);
            m_right = glm::normalize(glm::cross(m_direction, m_worldUp));
            m_up = glm::normalize(glm::cross(m_right, m_direction));
            m_view = glm::lookAt(m_pos, m_target, m_up);
            break;
    }
}

void Camera::Reset()
{
    Init();
}

void Camera::SetMode(const CameraMode mode)
{
    m_mode = mode;
    switch (mode)
    {
        case CameraMode::FPS:
            m_yaw = std::atan2(m_direction.z, m_direction.x);
            m_pitch = glm::asin(m_direction.y);
            m_roll = std::atan2(glm::dot(m_right, m_worldUp), glm::dot(m_up, m_worldUp));
            break;
        case CameraMode::ORBIT:
            break;
    }
}
