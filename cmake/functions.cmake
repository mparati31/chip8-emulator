function(get_version_from_git OUTPUT_VAR)
    find_package(Git QUIET)
    
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(NOT GIT_VERSION)
            set(GIT_VERSION "unknown")
        endif()
    else()
        set(GIT_VERSION "unknown")
    endif()
    
    set(${OUTPUT_VAR} ${GIT_VERSION} PARENT_SCOPE)
endfunction()
