# author    Dubsky Tomas

include("cmake/RealProject/Utility.cmake")

function(_add_shader_compilation_target target)
    # Check if this target has shaders
    _does_target_have_shaders(${target})
    if(NOT ${target_has_shaders})
        return()
    endif()

    # Load target properties and prepare variables
    get_target_property(base_dir ${target} realproject_base_dir_rel)
    get_target_property(shader_sources_rel ${target} realproject_glsl_sources_rel)
    get_target_property(target_includes ${target} INCLUDE_DIRECTORIES)
    if (NOT shader_sources_rel)
        return()
    endif()
    _load_shader_naming_constants()

    # Determine shader compilation flags
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(glslc_flags "-O" "-DNDEBUG")
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        set(glslc_flags "-g" "-DNDEBUG")
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        set(glslc_flags "-Os" "-DNDEBUG")
    else()
        set(glslc_flags "-O0" "-g")
        if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            message(WARNING "Configuting unknown configuration
            (not Release, Debug, RelWithDebInfo or MinSizeRel). 
            GLSL will be compiled as Debug.")
        endif()
    endif()
    get_target_property(glsl_standard ${target} realproject_glsl_standard)
    list(APPEND glslc_flags "-std=${glsl_standard}")

    # Create commands to compile the shaders
    foreach(shader_source_rel IN LISTS shader_sources_rel)
        get_filename_component(shader_ext ${shader_source_rel} LAST_EXT)
        if (${shader_ext} IN_LIST REALPROJECT_GLSL_STAGE_EXTENSIONS)
            set(shader_source_abs "${CMAKE_CURRENT_SOURCE_DIR}/${base_dir}/${shader_source_rel}")
            set(shader_out_base_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${shader_source_rel}")
            set(shader_c_abs    "${shader_out_base_abs}.${shader_c_ext}")   # C representation
            set(shader_dep_abs  "${shader_out_base_abs}.${shader_dep_ext}") # Dependency file
            set(shader_txt_abs  "${shader_out_base_abs}.${shader_txt_ext}") # Text disassembly
            set(shader_bin_abs  "${shader_out_base_abs}.${shader_bin_ext}") # Binary representation
            list(APPEND shader_bins_abs ${shader_c_abs})
            set(shader_dep_abs "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${shader_source_rel}.d")
            get_filename_component(shader_bin_dir_abs ${shader_c_abs} DIRECTORY)
            file(MAKE_DIRECTORY ${shader_bin_dir_abs})
            add_custom_command(
                OUTPUT ${shader_c_abs} ${shader_txt_abs} ${shader_bin_abs}
                COMMAND ${Vulkan_GLSLC_EXECUTABLE}  # C + dependency
                        -MD -mfmt=c -MF ${shader_dep_abs} ${shader_source_abs}
                        -o ${shader_c_abs} --target-env=vulkan1.3 ${glslc_flags}
                        "$<LIST:TRANSFORM,${target_includes},PREPEND,-I>"
                COMMAND ${Vulkan_GLSLC_EXECUTABLE}  # Disassembly
                        -S ${shader_source_abs}
                        -o ${shader_txt_abs} --target-env=vulkan1.3 ${glslc_flags}
                        "$<LIST:TRANSFORM,${target_includes},PREPEND,-I>"
                COMMAND ${Vulkan_GLSLC_EXECUTABLE}  # Binary
                        ${shader_source_abs}
                        -o ${shader_bin_abs} --target-env=vulkan1.3 ${glslc_flags}
                        "$<LIST:TRANSFORM,${target_includes},PREPEND,-I>"
                DEPENDS ${shader_source_abs}
                BYPRODUCTS ${shader_dep_abs}
                COMMENT "Compiling shader: ${shader_source_rel}"
                DEPFILE ${shader_dep_abs}
                VERBATIM
                COMMAND_EXPAND_LISTS
            )
        endif()
    endforeach()

    # Add custom target which compiles the shaders
    set(shader_target "${target}${shader_target_suffix}")
    add_custom_target(${shader_target} DEPENDS ${shader_bins_abs})
    add_dependencies(${target} ${shader_target})
endfunction()