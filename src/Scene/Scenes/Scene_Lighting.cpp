#include "Scene_Lighting.h"

#include "../../Core/IncludeAll.h"
#include "../../Extra/Camera.h"

namespace Scene
{
    Scene_Lighting::Scene_Lighting() = default;
    Scene_Lighting::~Scene_Lighting() = default;

    int Scene_Lighting::Init()
    {
        int error = 0;

        ShaderFilePath pathsObj = {
            BASE_PATH / "resources/shaders/Lighting/Cube_VERT.shader",
            BASE_PATH / "resources/shaders/Lighting/Cube_FRAG.shader"
        };

        shaderObj_ = std::make_unique<Shader>(pathsObj, error);
        if (error) return error;
        shaderObj_->Bind();

        ShaderFilePath pathsLight = {
            BASE_PATH / "resources/shaders/Lighting/Light_VERT.shader",
            BASE_PATH / "resources/shaders/Lighting/Light_FRAG.shader"
        };

        shaderLight_ = std::make_unique<Shader>(pathsLight, error);
        if (error) return error;
        shaderLight_->Bind();

        std::vector<float> verticesObj = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);

        vaObj_ = std::make_unique<VertexArray>();
        vaObj_->Bind();
        vbCube_ = std::make_unique<VertexBuffer>(verticesObj);
        vbCube_->Bind();
        vaObj_->AddBuffer(*vbCube_, layout);

        vaLight_ = std::make_unique<VertexArray>();
        vaLight_->Bind();
        vaLight_->AddBuffer(*vbCube_, layout);

        camera_ = std::make_unique<Camera>(CameraMode::ORBIT);
        camera_->SetPosition({5.0f, 5.0f, 5.0f});
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
        camera_->enableMouseControl = true;

        modelObj_ = {1.0f};
        modelObj_ = glm::translate(modelObj_, glm::vec3{0.0f, 0.0f, 0.0f});
        modelLight_ = {1.0f};
        modelLight_ = glm::translate(modelObj_, glm::vec3{0.0f, 3.0f, 4.0f});
        modelLight_ = glm::scale(modelLight_, glm::vec3(0.2f));

        speed_ = 1.0f;
        moving_ = true;
        debug_ = false;
        lockCam_ = false;
        pow_ = 32;
        strength_ = 0.5;

        return error;
    }

    void Scene_Lighting::Update(float deltaTime)
    {
        static float totalTime = 0.0f;
        float radius = 5.0f;
        if (moving_)
        {
        totalTime += deltaTime;
        float angle = totalTime * speed_;
        glm::vec3 pos;
        pos.x = radius * cos(angle);
        pos.y = radius * cos(angle) * sin(angle);
        pos.z = radius * sin(angle);
        modelLight_ = glm::mat4(1.0f);
        modelLight_ = glm::translate(modelLight_, pos);
        }


        HandleInput(deltaTime);
        camera_->Update();
    }

    void Scene_Lighting::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& renderer = sceneManager_->GetRenderer();

        // Test Object
        shaderObj_->Bind();
        shaderObj_->SetUniformMat4f("u_view", camera_->GetViewMatrix());
        shaderObj_->SetUniformMat4f("u_proj", camera_->GetProjectionMatrix());
        vaObj_->Bind();
        shaderObj_->SetUniformMat4f("u_model", modelObj_);
        shaderObj_->SetUniformVec3("u_ObjColor", coral);
        shaderObj_->SetUniformVec3("u_LightColor", lightColor_);
        shaderObj_->SetUniformVec3("u_LightPos", glm::vec3(modelLight_[3]));
        shaderObj_->SetUniformVec3("u_ViewPos", camera_->GetPosition());
        shaderObj_->SetUniform1i("u_pow", pow_);
        shaderObj_->SetUniform1f("u_specularStrength", strength_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderer.Draw(*vaObj_, *shaderObj_, 36);
        if (debug_)
            shaderObj_->SetUniform1i("u_Debug", 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderer.Draw(*vaObj_, *shaderObj_, 36);
        shaderObj_->SetUniform1i("u_Debug", 0);

        // Light source
        shaderLight_->Bind();
        shaderLight_->SetUniformMat4f("u_view", camera_->GetViewMatrix());
        shaderLight_->SetUniformMat4f("u_proj", camera_->GetProjectionMatrix());
        vaLight_->Bind();
        shaderLight_->SetUniformMat4f("u_model", modelLight_);
        shaderLight_->SetUniformVec3("u_LightColor", lightColor_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        renderer.Draw(*vaObj_, *shaderLight_, 36);
    }

    void Scene_Lighting::ImGuiRender()
    {
        cameraModeIndex_ = static_cast<int>(camera_->GetMode());

        ImGui::Begin("Camera Control");

        if (ImGui::Button("back"))
            sceneManager_->SetScene("Menu");

        ImGui::SameLine(0.0f, 5.0f);
        if (ImGui::Button("reset Camera"))
            camera_->Reset();

        if (ImGui::Combo("Camera Mode", &cameraModeIndex_, Camera::GetCameraModes().data(), Camera::GetCameraModes().size()))
            camera_->SetMode(static_cast<CameraMode>(cameraModeIndex_));

        ImGui::Checkbox("Debug", &debug_);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Checkbox("Moving", &moving_);
        ImGui::Checkbox("LockCam", &lockCam_);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::SliderFloat("Speed", &speed_, 0.1f, 5.0f);
        ImGui::SliderInt("Power", &pow_, 0, 256);
        ImGui::SliderFloat("Strength", &strength_, 0.0f, 1.0f);
        ImGui::ColorPicker3("Light Color", &lightColor_[0]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    void Scene_Lighting::OnEnter()
    {
        glEnable(GL_DEPTH_TEST);
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        double windowWidth = mode->width * 0.6;
        double windowHeight = mode->height * 0.8;
        glfwSetWindowSize(&renderer_->GetWindow(), windowWidth, windowHeight);
        double posX = (mode->width - windowWidth) / 2;
        double posY = (mode->height - windowHeight) / 2;
        glfwSetWindowPos(&renderer_->GetWindow(), posX, posY);
        double x, y;
        glfwGetCursorPos(&renderer_->GetWindow(), &x, &y);
        camera_->SyncMouse(x, y);
        Scene::OnEnter();
    }

    void Scene_Lighting::OnLeave()
    {
        shaderObj_->Unbind();
        GLCall(glDisable(GL_DEPTH_TEST));
        renderer_->ReleaseMouse();
        glfwSetWindowSize(&renderer_->GetWindow(), renderer_->GetDefaultWindowWidth(), renderer_->GetDefaultWindowHeight());
        Scene::OnLeave();
    }

    void Scene_Lighting::HandleInput(float deltaTime)
    {
        GLFWwindow* window = &sceneManager_->GetRenderer().GetWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            sceneManager_->SetScene("Menu");
        }

        if (!lockCam_)
        {
            camera_->HandleGenericCameraControls(window, deltaTime);
        }

        if (camera_->GetMode() != CameraMode::FPS || glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderer_->ReleaseMouse();
            camera_->enableMouseControl = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderer_->CaptureMouse();
            camera_->enableMouseControl = true;
        }
    }

    void Scene_Lighting::OnResize(int width, int height)
    {
        camera_->SetAspectRatio(static_cast<float>(renderer_->GetWindowWidth()) / static_cast<float>(renderer_->GetWindowHeight()));
    }
}
