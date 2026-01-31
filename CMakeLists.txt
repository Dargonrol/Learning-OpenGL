cmake_minimum_required(VERSION 3.0...${CMAKE_VERSION})
project(OpenGLAPP VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
        GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
        GLM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(GLM)

add_executable(${PROJECT_NAME}
        include/imgui/imgui.h
        include/imgui/imgui.cpp
        include/imgui/imconfig.h
        include/imgui/imgui_demo.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_internal.h
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_glfw.h
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.h
        include/imgui/backends/imgui_impl_opengl3.cpp

        src/main.cpp
        src/Renderer.h
        src/Renderer.cpp
        src/Util.h
        src/VertexBuffer.cpp
        src/VertexBuffer.h
        src/IndexBuffer.cpp
        src/IndexBuffer.h
        src/VertexArray.cpp
        src/VertexArray.h
        src/VertexBufferLayout.h
        src/Shader.cpp
        src/Shader.h
        include/stb_image/stb_image.h
        src/Texture.cpp
        src/Texture.h
        src/stb_image.cpp
        src/Scene/Scene.h
        src/Scene/Scenes/Scene_ClearColor.cpp
        src/Scene/Scenes/Scene_ClearColor.h
        src/Scene/Scenes/Scene_Menu.cpp
        src/Scene/Scenes/Scene_Menu.h
        src/Scene/SceneManager.cpp
        src/Scene/SceneManager.h
        src/Scene/Scenes/Scene_Texture2D.cpp
        src/Scene/Scenes/Scene_Texture2D.h
        src/Vertex.h
        src/Scene/Scenes/Scene_BatchRendering.cpp
        src/Scene/Scenes/Scene_BatchRendering.h
        src/Scene/Scene.cpp
        src/Scene/Scenes/Scene_Simple3D.cpp
        src/Scene/Scenes/Scene_Simple3D.h
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE glfw glm
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include/imgui
        ${PROJECT_SOURCE_DIR}/include/imgui/backends
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        ${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}"
)

