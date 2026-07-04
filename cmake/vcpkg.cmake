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