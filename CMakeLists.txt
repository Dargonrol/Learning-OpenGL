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

add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE glfw
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        ${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}"
)

