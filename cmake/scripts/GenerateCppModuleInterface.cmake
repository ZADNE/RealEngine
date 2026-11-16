# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_generate_cpp_interface_module target)
    get_target_property(module_name ${target} realproject_cxx_interface_module)
    if("${module_name}" STREQUAL "")
        return() # Target does not use generated module interface
    endif()

    # Configure the file
    get_property(module_part_paths
        FILE_SET realproject_module_interfaces
        TARGET ${target}
        PROPERTY SOURCES
    )
    get_target_property(base_dir_abs ${target} realproject_base_dir_abs)
    set(module_base_dir "${base_dir_abs}/${module_name}")
    set(exports_imports "")
    foreach(module_part_path IN LISTS module_part_paths)
        cmake_path(IS_PREFIX module_base_dir ${module_part_path} is_in_source_dir)
        if(NOT ${is_in_source_dir})
            continue()
        endif()
        cmake_path(
            RELATIVE_PATH module_part_path
            BASE_DIRECTORY ${module_base_dir}
        )
        cmake_path(REMOVE_EXTENSION module_part_path LAST_ONLY)
        string(REPLACE "/" "." module_part_path ${module_part_path})
        string(APPEND exports_imports "export import :${module_part_path};\n")
    endforeach()
    get_target_property(target_binary_dir ${target} BINARY_DIR)
    get_target_property(base_dir_rel ${target} realproject_base_dir_rel)
    set(interface_module "${target_binary_dir}/${base_dir_rel}/${target}.cppm")
    _generate_configured_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates/ModuleInterface.cppm.in"
        ${interface_module}
    )

    # Make the file part of target
    target_sources(${target}
        PUBLIC
            FILE_SET realproject_module_interfaces
            FILES ${interface_module}
    )
endfunction()