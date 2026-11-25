# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_generate_cpp_setup_file target)
    # Check whether it should be generated
    get_target_property(setup_rel ${target} RE_SETUP_FILEPATH_REL)
    get_target_property(module_name ${target} RE_SETUP_MODULE)
    if(("${setup_rel}" STREQUAL "") OR ("${module_name}" STREQUAL ""))
        if(NOT "${setup_rel}${module_name}" STREQUAL "")
            message(WARNING "Project setup for ${target} will not be generated "
                            "because both target properties 'RE_SETUP_FILEPATH_REL' "
                            "and 'RE_SETUP_MODULE' must be set."
            )
        endif()
        return()
    endif()

    # Prepare variables
    get_target_property(base_dir ${target} RE_BASE_DIR_REL)
    get_target_property(glsl_header_extensions ${target} RE_GLSL_HEADER_EXTENSIONS)
    get_target_property(glsl_stage_extensions ${target} RE_GLSL_STAGE_EXTENSIONS)
    set(template_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates")
    set(cmake_command ${CMAKE_COMMAND})
    set(source_dir ${CMAKE_SOURCE_DIR})
    set(binary_dir ${CMAKE_BINARY_DIR})
    set(shader_file_extensions "${glsl_header_extensions};${glsl_stage_extensions}")
    file(RELATIVE_PATH target_base_dir_rel ${CMAKE_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}")
    set(cppm_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${setup_rel}")

    # Generate the file
    _re_generate_configured_file(
        "${template_dir}/TargetSetup.cppm.in"
        ${cppm_abs}
    )
    target_sources(${target}
        PUBLIC
            FILE_SET re_public_module_interfaces
            FILES ${cppm_abs}
    )
endfunction()