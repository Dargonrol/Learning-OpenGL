function(enable_warnings target)
    if(NOT ENABLE_WARNINGS)
        return()
    endif()

    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(ENABLE_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wno-unused-parameter
        )

        if(ENABLE_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()

endfunction()