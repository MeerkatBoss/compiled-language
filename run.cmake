function(add_run_target target)
    add_custom_target(run_${target} DEPENDS ${target}
                        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} "$(ARGS)"
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMENT "Running target ${target}")
endfunction()
