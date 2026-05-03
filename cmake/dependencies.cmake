# glfw
if(USE_SYSTEM_GLFW)
    find_package(glfw3 QUIET)
endif()

if(NOT glfw3_FOUND)
    message(STATUS "Using vendored GLFW")

    add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glfw)
else()
    message(STATUS "Using system GLFW")
endif()

# glad
add_library(glad STATIC
        vendor/glad/src/glad.c
)

target_include_directories(glad PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/glad/include>
)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
        ${CMAKE_SOURCE_DIR}/vendor/glm
)

# imgui
add_library(imgui STATIC
        vendor/imgui/imgui.h
        vendor/imgui/imgui.cpp
        vendor/imgui/imconfig.h
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_internal.h
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/backends/imgui_impl_glfw.h
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_opengl3.h
        vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/vendor/imgui
        ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
)

# stb image
add_library(stb_image STATIC
        vendor/stb/stb_image.cpp
)

target_include_directories(stb_image PUBLIC
        ${CMAKE_SOURCE_DIR}/vendor/stb
)