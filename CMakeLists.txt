cmake_minimum_required(VERSION 3.0...${CMAKE_VERSION})
project(OpenGLAPP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
        GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG v2.0.8
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_MakeAvailable(glad)
    add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/linmath.h
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE glfw glad
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${glad_SOURCE_DIR}/include
        ${glad_BINARY_DIR}/include
)

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE WORKDIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
        ${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}"
)

