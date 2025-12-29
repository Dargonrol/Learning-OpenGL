#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <filesystem>

#include <iostream>
#include <stdio.h>

#include "Util.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

#ifndef SOURCE_DIR
    #define SOURCE_DIR
#endif
#define BASE_PATH std::filesystem::path(SOURCE_DIR)

int main()
{
    float positions[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f,  -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    Renderer renderer;
    renderer.Init();

    VertexArray va;

    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    vb.Bind();

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);
    ib.Bind();

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

    float r = 0.0f;
    float inc = 0.05f;
    while (!renderer.GetWindowShouldClose())
    {
        renderer.Clear();
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.5f, 0.5f, 1.0f);
        renderer.Draw(va, ib, shader); // will eventually be va, ib, material

        if (r > 1.0f)
            inc = -0.05f;
        else if (r < 0.0f)
            inc = 0.05f;

        r += inc;

        renderer.Update();
    }

    renderer.Shutdown();
    exit(EXIT_SUCCESS);
}