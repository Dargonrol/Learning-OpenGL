cmake_minimum_required(VERSION 3.5)
project(OpenGLAPP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
        GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG master
)
FetchContent_MakeAvailable(GLFW)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG master
)
FetchContent_MakeAvailable(glad)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE glfw glad
)

target_include_directories(${PROJECT_NAME} PRIVATE ${glad_SOURCE_DIR}/include)

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE WORKDIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
        ${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}"
)

