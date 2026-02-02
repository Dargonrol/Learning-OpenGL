#include "Camera.h"

#include "GLFW/glfw3.h"

Camera::Camera(const CameraMode mode)
{
    m_mode = mode;
    Init();
    SetMode(mode); // to initialize
}

void Camera::Init()
{
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

void Camera::SetFOV(const float fov) { m_perspectiveData.fov = fov; }
void Camera::SetNear(const float near) { m_perspectiveData.near = near; m_orthoData.near = near; }
void Camera::SetFar(const float far) { m_perspectiveData.far = far; m_orthoData.far = far; }
void Camera::SetAspectRatio(const float ratio) { m_perspectiveData.aspect = ratio; }
void Camera::SetOrthoZoom(const float zoom) { m_orthoData.zoom = zoom; }
void Camera::SetOrthoBounds(const OrthoData::OrthoBounds bounds) { m_orthoData.bounds = bounds; }
void Camera::SetOrthoBounds(const float left, const float right, const float bottom, const float top) { m_orthoData.bounds.left = left; m_orthoData.bounds.right = right; m_orthoData.bounds.top = top; m_orthoData.bounds.bottom = bottom; }
void Camera::SetOrthoData(const OrthoData &ortho_data) { m_orthoData = ortho_data; }
void Camera::SetPerspectiveData(const PerspectiveData& perspective_data) { m_perspectiveData = perspective_data; m_orthoData.near = perspective_data.near; m_orthoData.far = perspective_data.far; }
void Camera::SetProjectionMatrix(const glm::mat4 &proj) { m_proj = proj; }

float Camera::GetFOV() const { return m_perspectiveData.fov; }
float Camera::GetNear() const { return m_perspectiveData.near; }
float Camera::GetFar() const { return m_perspectiveData.far; }
float Camera::GetAspectRatio() const { return m_perspectiveData.aspect; }
float Camera::GetOrthoZoom() const { return m_orthoData.zoom; }
OrthoData::OrthoBounds & Camera::GetOrthoBounds() { return m_orthoData.bounds; }
OrthoData & Camera::GetOrthoData() { return m_orthoData; }
PerspectiveData & Camera::GetPerspectiveData() { return m_perspectiveData; }
glm::mat4 & Camera::GetProjectionMatrix() { return m_proj; }

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

            m_proj = glm::perspective(
                m_perspectiveData.fov,
                m_perspectiveData.aspect,
                m_perspectiveData.near,
                m_perspectiveData.far
                );
            break;

        case CameraMode::ORBIT:
            m_direction = glm::normalize(m_target - m_pos);
            m_right = glm::normalize(glm::cross(m_direction, m_worldUp));
            m_up = glm::normalize(glm::cross(m_right, m_direction));
            m_view = glm::lookAt(m_pos, m_target, m_up);

            m_proj = glm::perspective(
                m_perspectiveData.fov,
                m_perspectiveData.aspect,
                m_perspectiveData.near,
                m_perspectiveData.far
                );
            break;

        case CameraMode::ORTHO:
            m_view = glm::translate(glm::mat4(1.0f), -m_pos);

            if (m_orthoData.zoom <= 0.1f)
                m_orthoData.zoom = 0.1f;

            float cx = (m_orthoData.bounds.left + m_orthoData.bounds.right) / 2.0f;
            float cy = (m_orthoData.bounds.bottom + m_orthoData.bounds.top) / 2.0f;

            float halfWidth  = (m_orthoData.bounds.right - m_orthoData.bounds.left) / 2.0f * m_orthoData.zoom;
            float halfHeight = (m_orthoData.bounds.top   - m_orthoData.bounds.bottom) / 2.0f * m_orthoData.zoom;

            m_proj = glm::ortho(
                cx - halfWidth,
                cx + halfWidth,
                cy - halfHeight,
                cy + halfHeight,
                m_orthoData.near, m_orthoData.far
                );
            break;
    }
}

void Camera::Reset()
{
    Init();
    m_orthoData.zoom = 1.0f;
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
            m_proj = glm::perspective(
                m_perspectiveData.fov,
                m_perspectiveData.aspect,
                m_perspectiveData.near,
                m_perspectiveData.far
                );
            break;
        case CameraMode::ORBIT:
            m_proj = glm::perspective(
                m_perspectiveData.fov,
                m_perspectiveData.aspect,
                m_perspectiveData.near,
                m_perspectiveData.far
                );
            break;

        case CameraMode::ORTHO:
            m_direction= {0.0f, 0.0f, -1.0f};
            m_up= {0.0f, 1.0f,  0.0f};
            m_right= {1.0f, 0.0f,  0.0f};
            m_pos = {0.0f, 0.0f, 0.0f};

            float cx = (m_orthoData.bounds.left + m_orthoData.bounds.right) / 2.0f;
            float cy = (m_orthoData.bounds.bottom + m_orthoData.bounds.top) / 2.0f;

            float halfWidth  = (m_orthoData.bounds.right - m_orthoData.bounds.left) / 2.0f * m_orthoData.zoom;
            float halfHeight = (m_orthoData.bounds.top   - m_orthoData.bounds.bottom) / 2.0f * m_orthoData.zoom;

            m_proj = glm::ortho(
                cx - halfWidth,
                cx + halfWidth,
                cy - halfHeight,
                cy + halfHeight,
                m_orthoData.near, m_orthoData.far
                );
            break;
    }
}

/**
 * Handles generic camera movement and rotation based on keyboard input.\n
 *
 * Controls:\n
 *   Movement:\n
 *     W       - Move forward\n
 *     S       - Move backward\n
 *     A       - Move left\n
 *     D       - Move right\n
 *     SPACE   - Move upward\n
 *     LEFT_SHIFT, C, or LEFT_CONTROL - Move downward\n
 *
 *   Rotation:\n
 *     UP_ARROW    - Pitch up\n
 *     DOWN_ARROW  - Pitch down\n
 *     LEFT_ARROW  - Yaw left\n
 *     RIGHT_ARROW - Yaw right\n
 */
void Camera::HandleGenericCameraControls(GLFWwindow *window, float deltaTime, float camSpeed, float camSensitivity)
{
    switch (m_mode)
    {
        case CameraMode::FPS:
            HandleGenericCameraControlsFPS(window, deltaTime, camSpeed, camSensitivity);
            break;

        case CameraMode::ORBIT:
            HandleGenericCameraControlsFPS(window, deltaTime, camSpeed, camSensitivity);
            break;

        case CameraMode::ORTHO:
            HandleGenericCameraControlsOrtho(window, deltaTime, camSpeed);
            break;

        default:
            HandleGenericCameraControlsFPS(window, deltaTime, camSpeed, camSensitivity);
    }
}

void Camera::HandleGenericCameraControlsFPS(GLFWwindow *window, float deltaTime, float camSpeed, float camSensitivity)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        AddPosition(m_direction * camSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        AddPosition(-m_right * camSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        AddPosition(-m_direction * camSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        AddPosition(m_right * camSpeed * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        AddPosition(m_up * camSpeed * deltaTime);
    if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) | glfwGetKey(window, GLFW_KEY_C) | glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) == GLFW_PRESS)
        AddPosition(-m_up * camSpeed * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        AddPitch(camSensitivity * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        AddPitch(-camSensitivity * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        AddYaw(-camSensitivity * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        AddYaw(camSensitivity * deltaTime);
}

void Camera::HandleGenericCameraControlsOrtho(GLFWwindow *window, float deltaTime, float camSpeed)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_pos.y += camSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_pos.x -= camSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_pos.y -= camSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_pos.x += camSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_orthoData.zoom += 0.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_orthoData.zoom -= 0.5f * deltaTime;
}