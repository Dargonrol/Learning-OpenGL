cmake_minimum_required(VERSION 3.12...${CMAKE_VERSION})
project(OpenGLAPP VERSION 0.7 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(CMAKE_BUILD_TYPE)
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build type: multi-config (${CMAKE_CONFIGURATION_TYPES})")
endif()
message(STATUS "C++ compiler id: ${CMAKE_CXX_COMPILER_ID}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(options)
include(dependencies)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.h"
        "src/*.hpp"
        "src/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# precompiled headers
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <iostream>
        <vector>
        <string>
        <memory>
        <optional>
        <filesystem>
        <glad/glad.h>
        <GLFW/glfw3.h>
        <glm/glm.hpp>
        <spdlog/spdlog.h>
        <imgui.h>
)

# this allows #include "Core/Renderer.h" instead of #include "../../Core/Renderer.h"
target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb"
)


set_target_properties( ${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
        SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PROJECT_NAME="${PROJECT_NAME}"
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)

target_link_libraries( ${PROJECT_NAME} PRIVATE
        glfw
        glad
        glm::glm
        imgui
        imgui_backends
        assimp::assimp
        nlohmann_json::nlohmann_json
        fmt
        spdlog
)

include(compiler_warnings)
enable_warnings("${PROJECT_NAME}")