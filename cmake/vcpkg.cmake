set(VCPKG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg")

# 1. Fetch the Git submodule automatically if the folder is empty
if(NOT EXISTS "${VCPKG_DIR}/CMakeLists.txt")
    message(STATUS "vcpkg submodule is missing. Attempting to clone...")
    find_package(Git REQUIRED)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init -- vendor/vcpkg
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# 2. Bootstrap the vcpkg executable if it does not exist
if(NOT EXISTS "${VCPKG_DIR}/vcpkg${CMAKE_EXECUTABLE_SUFFIX}")
    message(STATUS "vcpkg executable not found. Starting automated bootstrapping...")
    if(WIN32)
        execute_process(
                COMMAND bootstrap-vcpkg.bat
                WORKING_DIRECTORY "${VCPKG_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )
    else()
        execute_process(
                COMMAND ./bootstrap-vcpkg.sh
                WORKING_DIRECTORY "${VCPKG_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
endif()

# 3. CROSS-PLATFORM: Provide Ninja via vcpkg if it is missing from the system
find_program(NINJA_PATH ninja)
if(NOT NINJA_PATH AND NOT CMAKE_MAKE_PROGRAM)
    message(STATUS "Ninja was not found on the system. vcpkg will install Ninja as a build tool...")

    # This vcpkg command downloads Ninja cross-platform into the internal vcpkg tools directory
    execute_process(
            COMMAND "${VCPKG_DIR}/vcpkg${CMAKE_EXECUTABLE_SUFFIX}" fetch ninja
            WORKING_DIRECTORY "${VCPKG_DIR}"
            OUTPUT_VARIABLE VCPKG_NINJA_OUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY
    )

    # vcpkg 'fetch' returns the direct, absolute path to the downloaded Ninja binary
    if(EXISTS "${VCPKG_NINJA_OUT}")
        set(CMAKE_MAKE_PROGRAM "${VCPKG_NINJA_OUT}" CACHE FILEPATH "Path to ninja" FORCE)
        message(STATUS "Using vcpkg-internal Ninja version: ${CMAKE_MAKE_PROGRAM}")
    endif()
endif()

# ==============================================================================
# Automated Portable Compiler Setup for Standalone Windows Builds
# ==============================================================================
if(WIN32 AND NOT DEFINED ENV{CLION_IDE})
    find_program(GCC_PATH gcc)
    if(NOT GCC_PATH)
        set(PORTABLE_MINGW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/mingw")
        set(COMPILER_BIN_DIR "${PORTABLE_MINGW_DIR}/w64devkit/bin")

        if(NOT EXISTS "${COMPILER_BIN_DIR}/gcc.exe")
            message(STATUS "No system compiler found. Downloading portable MinGW (w64devkit)...")

            # Download the lightweight portable zip archive
            set(MINGW_URL "https://github.com/skeeto/w64devkit/releases/download/v2.0.0/w64devkit-2.0.0.zip")
            file(DOWNLOAD "${MINGW_URL}" "${PORTABLE_MINGW_DIR}/mingw.zip" SHOW_PROGRESS)

            # Extract it locally into vendor/mingw
            message(STATUS "Extracting portable compiler...")
            file(ARCHIVE_EXTRACT INPUT "${PORTABLE_MINGW_DIR}/mingw.zip" DESTINATION "${PORTABLE_MINGW_DIR}")
            file(REMOVE "${PORTABLE_MINGW_DIR}/mingw.zip")
        endif()

        # Inject the portable compiler paths directly into this CMake instance
        message(STATUS "Using portable MinGW toolchain: ${COMPILER_BIN_DIR}")
        set(CMAKE_C_COMPILER "${COMPILER_BIN_DIR}/gcc.exe" CACHE FILEPATH "C Compiler" FORCE)
        set(CMAKE_CXX_COMPILER "${COMPILER_BIN_DIR}/g++.exe" CACHE FILEPATH "C++ Compiler" FORCE)

        # Add to path so vcpkg internal builds can find 'make/ar/rc' tools
        cmake_path(CONVERT "${COMPILER_BIN_DIR}" TO_NATIVE_PATH_LIST NATIVE_BIN_DIR)
        set(ENV{PATH} "${NATIVE_BIN_DIR};$ENV{PATH}")
    endif()
endif()