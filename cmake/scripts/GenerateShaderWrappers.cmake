# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_shader_filename_to_identifier filename out_shader_identifier)
    string(REPLACE "." "_" filename ${filename})
    string(SUBSTRING ${filename} 0 1 first_char)
    string(SUBSTRING ${filename} 1 -1 remainder)
    string(TOLOWER ${first_char} first_char)
    set(${out_shader_identifier} "${first_char}${remainder}" PARENT_SCOPE)
endfunction()

function(_shader_path_to_module_partition path_rel  out_module_partition)
    string(FIND ${path_rel} "/" first_sep)
    math(INCREMENT first_sep)
    string(SUBSTRING ${path_rel} ${first_sep} -1 module_partition)
    string(REPLACE "." "_" module_partition ${module_partition})
    string(REPLACE "/" "." module_partition ${module_partition})
    set(${out_module_partition} ":${module_partition}_gen" PARENT_SCOPE)
endfunction()

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
    _shader_filename_to_identifier(${shader} shader_identifier)
    _shader_path_to_module_partition(${shader_rel} module_partition)

    # Generate the files
    _generate_configured_file("${template_dir}/Shader.cppm.in" ${cppm_abs})
    _generate_configured_file("${template_dir}/Shader.cpp.in" ${cpp_abs})

    # Make the files part of the project
    target_sources(${target}
        PUBLIC
            FILE_SET re_public_module_interfaces
            FILES ${cppm_abs}
        PRIVATE
            FILE_SET re_module_impl_units
            FILES ${cpp_abs}
    )
endfunction()

function(_generate_cpp_wrapper_to_expose_header target header_rel)
    # Check if this header should be exposed
    set(header_abs "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/${header_rel}")
    file(READ ${header_abs} header_content LIMIT 2047)
    string(FIND
        "${header_content}"
        "#include <realshaders/CppIntegration.glsl>"
        found_index
    )
    if(found_index EQUAL -1)
        return() # This header should not be exposed
    endif()

    # Compose variables and paths
    get_filename_component(header ${header_rel} NAME)
    get_filename_component(header_rel_dir ${header_rel} DIRECTORY)
    set(cppm_rel "${header_rel_dir}/${header}.gen.cppm")
    set(cppm_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${cppm_rel}")
    _shader_path_to_module_partition(${header_rel} module_partition)

    # Generate the file
    configure_file("${template_dir}/ShaderHeader.cppm.in" ${cppm_abs} @ONLY)
    target_sources(${target}
        PUBLIC
            FILE_SET re_public_module_interfaces
            FILES ${cppm_abs}
    )
endfunction()

function(_generate_shaders_wrappers target)
    # Check if this target has shaders
    _does_target_have_shaders(${target})
    if(NOT ${target_has_shaders})
        return()
    endif()
    get_target_property(sources_as_cxx_impl_units ${target} RE_SOURCES_AS_CXX_IMPL_UNITS)
    if(NOT ${sources_as_cxx_impl_units})
        message(WARNING "Target ${target} does not use C++ module implementation unit "
                        "which are required by RealShaders. The target will not have shaders.")
        return()
    endif()

    # Load target properties and prepare variables
    get_target_property(base_dir ${target} RE_BASE_DIR_REL)
    get_target_property(shader_sources_rel ${target} RE_GLSL_SOURCES_REL)
    get_target_property(shader_headers_rel ${target} RE_GLSL_HEADERS_REL)
    get_target_property(cpp_namespace ${target} RE_SHADER_NAMESPACE)
    get_target_property(module_name ${target} RE_CXX_INTERFACE_MODULE)
    _load_shader_naming_constants()

    # Prepare reused variables
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
        if (${shader_ext} IN_LIST RE_GLSL_STAGE_EXTENSIONS)
            _generate_cpp_wrapper_for_stage(${target} ${shader_rel})
            get_filename_component(shader_dir ${shader_rel} DIRECTORY)
            list(APPEND shader_dirs ${shader_dir}) # Collect directories
        endif()
    endforeach()

    # Generate directory stage wrappers
    list(REMOVE_DUPLICATES shader_dirs)
    foreach(dir IN LISTS shader_dirs) # For each dir
        _shader_path_to_module_partition("${dir}/AllShaders" module_partition)
        # Find all shaders in it
        set(exports_imports)
        foreach(shader_rel IN LISTS shader_sources_rel)
            get_filename_component(shader_dir ${shader_rel} DIRECTORY)
            if(${shader_dir} STREQUAL ${dir})
                _shader_path_to_module_partition(${shader_rel} shader_module_partition)
                list(APPEND exports_imports ${shader_module_partition})
            endif()
        endforeach()

        # Generate the file
        set(cppm_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${dir}/AllShaders.gen.cppm")
        _generate_configured_file(
            "${template_dir}/ModuleInterface.cppm.in"
            ${cppm_abs}
        )
        target_sources(${target}
            PUBLIC
                FILE_SET re_public_module_interfaces
                FILES ${cppm_abs}
        )
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