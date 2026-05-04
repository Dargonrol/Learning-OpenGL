# Build type fallback
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif ()

# optional global flags
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_SANITIZERS "Enable ASAN/UBSAN" OFF)
#option(BUILD_32BIT "Build 32-bit version" OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_UNITY_BUILD OFF)

option(USE_SYSTEM_GLFW "Use system-installed GLFW if available" ON)
option(USE_SYSTEM_ASSIMP "Use system-installed assimp if available" ON)