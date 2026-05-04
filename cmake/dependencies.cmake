# glfw
set(GLFW_TARGET "")

if(USE_SYSTEM_GLFW)
    find_package(glfw3 CONFIG QUIET)
    find_package(glfw CONFIG QUIET)
endif()

if(TARGET glfw)
    set(GLFW_TARGET glfw)
    message(STATUS "Using system GLFW")
elseif(TARGET glfw::glfw)
    set(GLFW_TARGET glfw::glfw)
    message(STATUS "Using system GLFW")
else()
    message(STATUS "Using vendored GLFW")

    add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glfw)

    set(GLFW_TARGET glfw)
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

# assimp
set(ASSIMP_TARGET "")
if(USE_SYSTEM_ASSIMP ANDc OFF)
    SET (ASSIMP_SOVERSION "${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}")
    find_package(assimp REQUIRED)
    set(ASSIMP_TARGET assimp::assimp)
    message(STATUS "Using system ASSIMP")
else()
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ARCHIVE_IMPORTER OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/assimp)
    set(ASSIMP_TARGET assimp)
    message(STATUS "Using vendor ASSIMP")
endif()

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(MINIZIP REQUIRED minizip)

# assimp
#add_subdirectory(vendor/assimp)