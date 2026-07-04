cmake_minimum_required(VERSION 3.12...${CMAKE_VERSION})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(vcpkg)

project(OpenGLAPP VERSION 0.5 LANGUAGES C CXX)

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

# activate compiler cache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# activate LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT error)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(STATUS "IPO / LTO not supported: ${error}")
endif()

include(options)
include(dependencies)
include(compiler_warnings)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.h"
        "src/*.hpp"
        "src/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# this allows #include "Core/Renderer.h" instead of #include "../../Core/Renderer.h"
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")


set_target_properties( ${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
)


target_compile_definitions( ${PROJECT_NAME} PRIVATE
        SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PROJECT_NAME="${PROJECT_NAME}"
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include"
)

target_link_libraries( ${PROJECT_NAME} PRIVATE
        glfw
        glad
        glm::glm
        imgui::imgui
        #assimp::assimp
)

enable_warnings("${PROJECT_NAME}")

