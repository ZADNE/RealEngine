# author    Dubsky Tomas

function(re_get_engine_base_dir_abs out_var)
    if(TARGET RealEngine)
        get_target_property(base_dir RealEngine
            HEADER_DIRS_re_public_headers
        )
    else()
        # Assume building from source
        set(base_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../src")
    endif()
    set(${out_var} "${base_dir}" PARENT_SCOPE)
endfunction()

# Convenience copy function of 7z.dll
function(re_copy_7z_dll_to_target target)
    if(WIN32)
        get_target_property(7z_dll_path 7z_dll IMPORTED_LOCATION)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${7z_dll_path}"
                "$<TARGET_FILE_DIR:${target}>"
        )
    endif()
endfunction()

function(re_does_target_have_shaders target out_var)
    get_target_property(target_type ${target} TYPE)
    get_target_property(shader_sources_rel ${target} RE_GLSL_SOURCES_REL)
    get_target_property(shader_headers_rel ${target} RE_GLSL_HEADERS_REL)
    list(APPEND all_sources ${shader_sources_rel})
    list(APPEND all_sources ${shader_headers_rel})
    list(LENGTH all_sources shader_count)
    if(${target_type} STREQUAL "INTERFACE_LIBRARY")
        if(${shader_count} GREATER 0)
            message(FATAL_ERROR "Target ${target}: Interface libraries cannot have shaders")
        endif()
        set(${out_var} NO PARENT_SCOPE)
    elseif(${shader_count} EQUAL 0)
        set(${out_var} NO PARENT_SCOPE)
    else()
        set(${out_var} YES PARENT_SCOPE)
    endif()
endfunction()

# Internal utility functions below

function(_re_generate_configured_file in_file out_file)
    set(newline "
")
    file(READ ${in_file} contents)
    string(CONFIGURE "${contents}" contents)
    file(GENERATE
        OUTPUT ${out_file}
        CONTENT "${contents}"
    )
endfunction()

function(_re_parse_string_constant_from_cpp cpp_code var_name)
    string(REGEX MATCH "${var_name} = \"([^\"]*)\"" _ ${cpp_code})
    set("${var_name}_VAL" "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

function(_re_load_shader_naming_constants)
    re_get_engine_base_dir_abs(engine_base_dir_abs)
    file(READ
        "${engine_base_dir_abs}/RealEngine/utility/details/CMakeConstants.cppm"
        constants_file
    )
    _re_parse_string_constant_from_cpp("${constants_file}" k_shaderTargetSuffix)
    set(shader_target_suffix ${k_shaderTargetSuffix_VAL} PARENT_SCOPE)
    _re_parse_string_constant_from_cpp("${constants_file}" k_shaderSPIRVCFileExt)
    set(shader_c_ext ${k_shaderSPIRVCFileExt_VAL} PARENT_SCOPE)
    _re_parse_string_constant_from_cpp("${constants_file}" k_shaderDepFileExt)
    set(shader_dep_ext ${k_shaderDepFileExt_VAL} PARENT_SCOPE)
    _re_parse_string_constant_from_cpp("${constants_file}" k_shaderSPIRVDisFileExt)
    set(shader_txt_ext ${k_shaderSPIRVDisFileExt_VAL} PARENT_SCOPE)
    _re_parse_string_constant_from_cpp("${constants_file}" k_shaderSPIRVBinFileExt)
    set(shader_bin_ext ${k_shaderSPIRVBinFileExt_VAL} PARENT_SCOPE)
endfunction()