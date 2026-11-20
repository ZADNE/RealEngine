# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_generate_cpp_setup_file target)
    # Check whether it should be generated
    get_target_property(setup_rel ${target} realproject_setup_filepath_rel)
    get_target_property(module_name ${target} realproject_setup_module)
    if(("${setup_rel}" STREQUAL "") OR ("${module_name}" STREQUAL ""))
        if(NOT "${setup_rel}${module_name}" STREQUAL "")
            message(WARNING "Project setup for ${target} will not be generated "
                            "because both target property 'realproject_setup_filepath_rel' "
                            "and 'realproject_setup_module' must be set."
            )
        endif()
        return()
    endif()

    # Prepare variables
    get_target_property(base_dir ${target} realproject_base_dir_rel)
    set(template_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates")
    set(cmake_command ${CMAKE_COMMAND})
    set(source_dir ${CMAKE_SOURCE_DIR})
    set(binary_dir ${CMAKE_BINARY_DIR})
    set(shader_file_extensions "${REALPROJECT_GLSL_HEADER_EXTENSIONS};${REALPROJECT_GLSL_STAGE_EXTENSIONS}")
    file(RELATIVE_PATH target_base_dir_rel ${CMAKE_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}")
    set(cppm_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${setup_rel}")

    # Generate the file
    _generate_configured_file(
        "${template_dir}/TargetSetup.cppm.in"
        ${cppm_abs}
    )
    target_sources(${target}
        PUBLIC
            FILE_SET realproject_public_module_interfaces
            FILES ${cppm_abs}
    )
endfunction()