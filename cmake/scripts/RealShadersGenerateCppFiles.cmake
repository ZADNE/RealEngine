# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_generate_cpp_wrapper_for_stage target shader_rel)
    # Compose variables
    get_filename_component(shader ${shader_rel} NAME)
    get_filename_component(shader_rel_dir ${shader_rel} DIRECTORY)
    set(cppm_rel "${shader_rel_dir}/${shader}.gen.cppm")
    set(cpp_rel "${shader_rel_dir}/${shader}.gen.cpp")
    set(cppm_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${cppm_rel}")
    set(cpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${cpp_rel}")
    file(RELATIVE_PATH target_base_dir_rel ${CMAKE_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}")
    file(TO_NATIVE_PATH "${target_base_dir_rel}/${shader_rel}" debug_shader_rel)
    string(REPLACE "\\" "\\\\" debug_shader_rel "${debug_shader_rel}")
    set(debug_shader_rel "$<$<CONFIG:Debug>:${debug_shader_rel}>")
    string(REPLACE "." "_" shader_ ${shader})
    string(TOLOWER ${shader_rel} shader_partition)
    string(FIND ${shader_partition} "/" first_sep)
    string(SUBSTRING ${shader_partition} 0 ${first_sep} module_name)
    math(INCREMENT first_sep)
    string(SUBSTRING ${shader_partition} ${first_sep} -1 module_partition)
    string(REPLACE "." "_" module_partition ${module_partition})
    string(REPLACE "/" "." module_partition ${module_partition})

    # Generate the files
    _generate_configured_file("${template_dir}/Shader.cppm.in" ${cppm_abs})
    _generate_configured_file("${template_dir}/Shader.cpp.in" ${cpp_abs})

    # Make the files part of the project
    target_sources(${target}
        PUBLIC
            FILE_SET realproject_module_interfaces
            FILES ${cppm_abs}
        PRIVATE
            FILE_SET realproject_module_impl_units
            FILES ${cpp_abs}
    )
endfunction()

function(_generate_cpp_wrapper_to_expose_header target header_rel)
    # Compose paths
    get_filename_component(header ${header_rel} NAME)
    get_filename_component(header_rel_dir ${header_rel} DIRECTORY)
    set(hpp_rel "${header_rel_dir}/${header}.gen.hpp")
    set(hpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${hpp_rel}")

    # Generate the file
    configure_file("${template_dir}/HeaderWrapper.hpp.in" ${hpp_abs} @ONLY)
endfunction()

function(_generate_cpp_wrappers_for_shaders target)
    # Check if this target has shaders
    _does_target_have_shaders(${target})
    if(NOT ${target_has_shaders})
        return()
    endif()
    get_target_property(sources_as_cxx_impl_units ${target} realproject_sources_as_cxx_impl_units)
    if(NOT ${sources_as_cxx_impl_units})
        message(WARNING "Target ${target} does not use C++ module implementation unit "
                        "which are required by RealShaders. The target will not have shaders.")
        return()
    endif()

    # Load target properties and prepare variables
    get_target_property(base_dir ${target} realproject_base_dir_rel)
    get_target_property(shader_sources_rel ${target} realproject_glsl_sources_rel)
    get_target_property(shader_headers_rel ${target} realproject_glsl_headers_rel)
    get_target_property(cpp_namespace ${target} realproject_shader_cxx_namespace)
    get_target_property(shaders_cxx_module ${target} realproject_shaders_cxx_module)
    _load_shader_naming_constants()

    # Prepare reusable variables
    if("${cpp_namespace}" STREQUAL "")
        set(cpp_export_namespace_start  "export {\n")
        set(cpp_export_namespace_end    "\n}")
        set(cpp_namespace_start         "")
        set(cpp_namespace_end           "")
    else()
        set(cpp_export_namespace_start  "export namespace ${cpp_namespace} {\n")
        set(cpp_export_namespace_end    "\n} // namespace ${cpp_namespace}")
        set(cpp_namespace_start         "namespace ${cpp_namespace} {\n")
        set(cpp_namespace_end           "\n} // namespace ${cpp_namespace}")
    endif()
    set(template_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates")
    
    # Generate stage wrappers
    set(shader_dirs)
    foreach(shader_rel IN LISTS shader_sources_rel)
        get_filename_component(shader_ext ${shader_rel} LAST_EXT)
        if (${shader_ext} IN_LIST REALPROJECT_GLSL_STAGE_EXTENSIONS)
            _generate_cpp_wrapper_for_stage(${target} ${shader_rel})
            get_filename_component(shader_dir ${shader_rel} DIRECTORY)
            list(APPEND shader_dirs ${shader_dir}) # Collect directories
        endif()
    endforeach()

    # Expose shader headers
    foreach(header_rel IN LISTS shader_headers_rel)
        _generate_cpp_wrapper_to_expose_header(${target} ${header_rel})
    endforeach()
    list(LENGTH shader_headers_rel exposed_header_count)
    if(${exposed_header_count} GREATER 0)
        # Add include path for RealShaders folder that header wrappers use
        get_realengine_base_dir_abs()
        target_include_directories(${target}
            PUBLIC "${realengine_base_dir_abs}"
        )
    endif()

    # Set include path so that the generated code can be included
    target_include_directories(${target} BEFORE
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${base_dir}>"
    )
endfunction()