#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <filesystem>
#include <fstream>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>
#include <sstream>

#ifndef SOURCE_DIR
#define SOURCE_DIR
#endif

#define BASE_PATH std::filesystem::path(SOURCE_DIR)


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << std::hex << error << std::dec << "\n";
    }
}

struct ShaderSourceCode
{
    std::string vertexShader;
    std::string fragmentShader;
};

struct ShaderFilePath
{
    std::filesystem::path vertexShader;
    std::filesystem::path fragmentShader;
};

static ShaderSourceCode parseShader(const ShaderFilePath& filepaths)
{
    std::string line;
    std::stringstream ss[2];

    std::ifstream stream(filepaths.vertexShader);
    if (!stream.is_open())
        std::cerr << "Failed to open vertex shader: " << std::filesystem::absolute(filepaths.vertexShader) << std::endl;
    while (getline(stream, line))
    {
        if (line.find("#shader") == std::string::npos)
        {
            ss[0] << line << '\n';
        }
    }

    std::ifstream stream2(filepaths.fragmentShader);
    if (!stream2.is_open())
        std::cerr << "Failed to open fragment shader: " << std::filesystem::absolute(filepaths.fragmentShader) << std::endl;
    while (getline(stream2, line))
        if (line.find("#shader") == std::string::npos)
        {
            ss[1] << line << '\n';
        }

    return {ss[0].str(), ss[1].str() };
};

static GLuint CompileShader(const std::string& source, GLuint type)
{
    std::string typeString;
    switch (type)
    {
        case GL_VERTEX_SHADER:
            typeString = "vertex";
            break;

        case GL_FRAGMENT_SHADER:
            typeString = "fragment";
            break;

        default:
            typeString = "unknown";
    }

    std::cout << "Compiling " << typeString << " shader" << std::endl;

    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::string message;
        message.resize(length);
        glGetShaderInfoLog(id, length, &length, message.data());

        std::cout << "failed to compile " << typeString.c_str() << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    std::cout << "Successfully compiled " << typeString << " shader" << std::endl;

    return id;
}

static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    GLuint fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // NOTE: OpenGL error checks have been omitted for brevity

    float positions[] = {
        -0.5f, -0.5f,
         0.5f,  -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    ShaderSourceCode shaders = parseShader(
        {
            BASE_PATH / "resources/shaders/basic.vertGL",
            BASE_PATH / "resources/shaders/basic.fragGL"
        });
    unsigned int shader = CreateShader(shaders.vertexShader, shaders.fragmentShader);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}