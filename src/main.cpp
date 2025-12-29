#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef SOURCE_DIR
    #define SOURCE_DIR ""
#endif
#define BASE_PATH std::filesystem::path(SOURCE_DIR)

int main()
{
    float vertices[] = {
        -50.0f, -50.0f, 0.0f, 0.0f,
         50.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  50.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    Renderer renderer(650, 500);
    renderer.Init();

    VertexArray va;

    VertexBuffer vb(vertices, 4 * 4 * sizeof(float));
    vb.Bind();

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);
    ib.Bind();

    glm::mat4 proj = glm::ortho(0.0f, (float)renderer.GetWindowWidth(), 0.0f, (float)renderer.GetWindowHeight(), -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    ShaderFilePath paths = {
        BASE_PATH / "resources/shaders/basic.vertGL",
        BASE_PATH / "resources/shaders/basic.fragGL"
    };

    Shader shader(paths);
    shader.Bind();
    shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.5f, 1.0f);

    Texture texture(BASE_PATH / "resources/textures/Nian.png");
    texture.Bind(0);
    shader.SetUniform1i("u_Texture", 0);

    glm::vec3 translationA(200, 200, 0);
    glm::vec3 translationB(400, 200, 0);

    float r = 0.0f;
    float inc = 0.05f;
    while (!renderer.GetWindowShouldClose())
    {
        // set up some kind of cache to see if shader is already bound. if so, don't run openGL call

        renderer.Clear();
        shader.Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader); // will eventually be va, ib, material
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader); // will eventually be va, ib, material
        }
        if (r > 1.0f)
            inc = -0.05f;
        else if (r < 0.0f)
            inc = 0.05f;

        r += inc;

        renderer.ImGui_BeginFrame();
        {
            ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);
            ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        renderer.ImGui_EndFrame();

        renderer.Update();
    }

    renderer.Shutdown();
    exit(EXIT_SUCCESS);
}