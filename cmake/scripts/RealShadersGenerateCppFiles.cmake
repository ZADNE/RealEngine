# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(_generate_cpp_wrapper_for_stage target shader_rel)
    # Compose paths
    get_filename_component(shader ${shader_rel} NAME)
    get_filename_component(shader_rel_dir ${shader_rel} DIRECTORY)
    set(hpp_rel "${shader_rel_dir}/${shader}.gen.hpp")
    set(cpp_rel "${shader_rel_dir}/${shader}.gen.cpp")
    set(hpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${hpp_rel}")
    set(cpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${cpp_rel}")
    file(RELATIVE_PATH target_base_dir_rel ${CMAKE_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}")
    file(TO_NATIVE_PATH "${target_base_dir_rel}/${shader_rel}" debug_shader_rel)
    string(REPLACE "\\" "\\\\" debug_shader_rel "${debug_shader_rel}")
    set(debug_shader_rel
        "$<$<CONFIG:Debug>:${debug_shader_rel}>")
    string(REPLACE "." "_" shader_ ${shader})

    # Generate files
    _generate_configured_file("${template_dir}/ShaderWrapper.hpp.in" ${hpp_abs})
    _generate_configured_file("${template_dir}/ShaderWrapper.cpp.in" ${cpp_abs})

    # Set the generated files as sources of the target
    target_sources(${target} PRIVATE ${hpp_abs} ${cpp_abs})
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

    # Load target properties and prepare variables
    get_target_property(base_dir ${target} realproject_base_dir_rel)
    get_target_property(shader_sources_rel ${target} realproject_glsl_sources_rel)
    get_target_property(shader_headers_rel ${target} realproject_glsl_headers_rel)
    get_target_property(cpp_namespace ${target} realproject_shader_cxx_namespace)
    _load_shader_naming_constants()

    # Prepare reusable variables
    if("${cpp_namespace}" STREQUAL "")
        set(cpp_namespace_start "")
        set(cpp_namespace_end "")
    else()
        set(cpp_namespace_start "namespace ${cpp_namespace} {\n")
        set(cpp_namespace_end "\n} // namespace ${cpp_namespace}")
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

    # Generate directory stage wrappers
    list(REMOVE_DUPLICATES shader_dirs)
    foreach(dir IN LISTS shader_dirs) # For each dir
        # Find all shaders in it
        set(shader_declarations)
        foreach(shader_rel IN LISTS shader_sources_rel)
            get_filename_component(shader_dir ${shader_rel} DIRECTORY)
            if(${shader_dir} STREQUAL ${dir})
                get_filename_component(shader ${shader_rel} NAME)
                string(REPLACE "." "_" shader_ ${shader})
                list(APPEND shader_declarations
                    "extern const re::ShaderSource ${shader_}\;"
                )
            endif()
        endforeach()

        # Generate the file
        list(JOIN shader_declarations "\n" shader_declarations)
        set(hpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${dir}/AllShaders.gen.hpp")
        configure_file("${template_dir}/AllShaders.hpp.in" ${hpp_abs} @ONLY)
        target_sources(${target} PRIVATE ${hpp_abs})
    endforeach()

    # Expose shader headers
    foreach(header_rel IN LISTS shader_headers_rel)
        _generate_cpp_wrapper_to_expose_header(${target} ${header_rel})
    endforeach()

    # Set include path so that the generated code can be included
    target_include_directories(${target} BEFORE
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/${base_dir}>"
    )
endfunction()