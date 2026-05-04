cmake_minimum_required(VERSION 3.0...${CMAKE_VERSION})
project(OpenGLAPP VERSION 0.4 LANGUAGES C CXX)

if(CMAKE_BUILD_TYPE)
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build type: multi-config (${CMAKE_CONFIGURATION_TYPES})")
endif()
message(STATUS "C++ compiler id: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result)

if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(options)
include(dependencies)
include(compiler_warnings)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Core/Renderer.h
        src/Core/Renderer.cpp
        src/Core/Util.h
        src/Core/VertexBuffer.cpp
        src/Core/VertexBuffer.h
        src/Core/IndexBuffer.cpp
        src/Core/IndexBuffer.h
        src/Core/VertexArray.cpp
        src/Core/VertexArray.h
        src/Core/VertexBufferLayout.h
        src/Core/Shader.cpp
        src/Core/Shader.h
        src/Core/Texture.cpp
        src/Core/Texture.h
        src/Scene/Scene.h
        src/Scene/Scenes/Scene_ClearColor.cpp
        src/Scene/Scenes/Scene_ClearColor.h
        src/Scene/Scenes/Scene_Menu.cpp
        src/Scene/Scenes/Scene_Menu.h
        src/Scene/SceneManager.cpp
        src/Scene/SceneManager.h
        src/Scene/Scenes/Scene_Texture2D.cpp
        src/Scene/Scenes/Scene_Texture2D.h
        src/Core/Vertex.h
        src/Scene/Scenes/Scene_BatchRendering.cpp
        src/Scene/Scenes/Scene_BatchRendering.h
        src/Scene/Scene.cpp
        src/Scene/Scenes/Scene_Simple3D.cpp
        src/Scene/Scenes/Scene_Simple3D.h
        src/Scene/Scenes/Scene_Simple3D_Camera.h
        src/Scene/Scenes/Scene_Simple3D_Camera.cpp
        src/Extra/Camera.cpp
        src/Extra/Camera.h
        src/OpenGL.h
        src/Core/VertexBufferLayout.cpp
        src/Scene/Scenes/Scene_Lighting.cpp
        src/Scene/Scenes/Scene_Lighting.h
        src/Core/IncludeAll.h
        src/Scene/Scenes/Scene_BasicPlane.cpp
        src/Scene/Scenes/Scene_BasicPlane.h
        src/Core/Material.h
        src/Extra/Object.h
        src/Core/ResourceManager.h
        src/Core/AssetPool.h
        src/Scene/Scenes/Scene_Material.cpp
        src/Scene/Scenes/Scene_Material.h
        src/Core/Material.cpp
        src/Extra/Objects/Cube.h
        src/Scene/Scenes/Scene_DiffuseMaps.cpp
        src/Scene/Scenes/Scene_DiffuseMaps.h
        src/Core/Parser.h
        src/Core/Parser.cpp
        src/Extra/Light.h
        src/Scene/Scenes/Scene_ModelLoading.cpp
        src/Scene/Scenes/Scene_ModelLoading.h
)


target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${GLFW_TARGET}
        glad
        glm
        imgui
        stb_image
        ${ASSIMP_TARGET}
)

#target_include_directories(${PROJECT_NAME} PRIVATE
#        ${MINIZIP_INCLUDE_DIRS}
#)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
        SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PROJECT_NAME="${PROJECT_NAME}"
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)

#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE ARCH_64)
#elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE ARCH_32)
#endif()

enable_warnings("${PROJECT_NAME}")

