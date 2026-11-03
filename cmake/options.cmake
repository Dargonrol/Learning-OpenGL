get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif ()

if(UNIX AND NOT APPLE)
    find_program(MOLD_EXECUTABLE NAMES mold)
    if(MOLD_EXECUTABLE)
        add_link_options("-fuse-ld=mold")
        message("found mold linker")
    endif()
endif()

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

option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_SANITIZERS "Enable ASAN/UBSAN" OFF)
set(CMAKE_UNITY_BUILD OFF)

option(USE_SYSTEM_GLFW "Use system-installed GLFW if available" ON)
option(USE_SYSTEM_ASSIMP "Use system-installed assimp if available" ON)