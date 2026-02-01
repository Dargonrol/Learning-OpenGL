#pragma once
#include "glm/gtc/type_ptr.hpp"

namespace Extra
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        [[nodiscard]] glm::mat4 GetViewMatrix() const;

    private:
        glm::mat4 m_view;
    };
}
