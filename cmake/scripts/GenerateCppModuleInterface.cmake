# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_re_generate_cpp_interface_module target)
    get_target_property(module_name ${target} RE_CXX_INTERFACE_MODULE)
    if("${module_name}" STREQUAL "")
        return() # Target does not use generated module interface
    endif()

    # Get path to module base in source and binary directories
    get_target_property(source_dir_abs ${target} RE_BASE_DIR_ABS)
    set(source_module_base_dir "${source_dir_abs}/${module_name}")
    get_target_property(binary_dir ${target} BINARY_DIR)
    get_target_property(base_dir_rel ${target} RE_BASE_DIR_REL)
    set(binary_module_base_dir "${binary_dir}/${base_dir_rel}/${module_name}")

    # Process all files that belong to the public file set
    get_property(module_part_paths
        FILE_SET re_public_module_interfaces
        TARGET ${target}
        PROPERTY SOURCES
    )
    foreach(module_part_path IN LISTS module_part_paths)
        if(source_module_base_dir PATH_IS_PREFIX module_part_path)
            set(module_base_dir ${source_module_base_dir})
        elseif(binary_module_base_dir PATH_IS_PREFIX module_part_path)
            set(module_base_dir ${binary_module_base_dir})
        else()
            message(FATAL_ERROR "File '${module_part_path}' is not inside source or binary directory. "
                                "Source: '${source_module_base_dir}', binary: '${binary_module_base_dir}'.")
        endif()
        cmake_path(
            RELATIVE_PATH module_part_path
            BASE_DIRECTORY ${module_base_dir}
        )
        cmake_path(REMOVE_EXTENSION module_part_path LAST_ONLY)
        string(REPLACE "." "_" module_part_path ${module_part_path})
        string(REPLACE "/" "." module_part_path ${module_part_path})
        list(APPEND exports_imports ":${module_part_path}")
    endforeach()

    # Configure the file
    get_target_property(target_binary_dir ${target} BINARY_DIR)
    get_target_property(base_dir_rel ${target} RE_BASE_DIR_REL)
    set(interface_module "${target_binary_dir}/${base_dir_rel}/${target}.cppm")
    _re_generate_configured_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates/ModuleInterface.cppm.in"
        ${interface_module}
    )

    # Make the file part of target
    target_sources(${target}
        PUBLIC
            FILE_SET re_public_module_interfaces
            FILES ${interface_module}
    )
endfunction()