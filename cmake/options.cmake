get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif ()

# Optionale globale Flags
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_SANITIZERS "Enable ASAN/UBSAN" OFF)
set(CMAKE_UNITY_BUILD OFF)

# Diese Optionen sind super für vcpkg:
option(USE_SYSTEM_GLFW "Use system-installed GLFW if available" ON)
option(USE_SYSTEM_ASSIMP "Use system-installed assimp if available" ON)