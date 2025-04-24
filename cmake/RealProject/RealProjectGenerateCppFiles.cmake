# author    Dubsky Tomas

include("cmake/RealProject/Utility.cmake")

function(_generate_cpp_setup_file target)
    # Load target properties and prepare variables
    get_target_property(base_dir ${target} realproject_base_dir_rel)
    get_target_property(setup_rel ${target} realproject_setup_filepath_rel)
    set(template_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/templates")
    set(cmake_command ${CMAKE_COMMAND})
    set(target_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
    set(binary_dir ${CMAKE_BINARY_DIR})

    # Generate the file
    _generate_configured_file(
        "${template_dir}/ProjectSetup.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${setup_rel}"
    )
endfunction()