# author    Dubsky Tomas

function(_generate_configured_file in_file out_file)
    file(READ ${in_file} contents)
    string(CONFIGURE "${contents}" contents)
    file(GENERATE
        OUTPUT ${out_file}
        CONTENT "${contents}"
    )
endfunction()

function(_does_target_have_shaders target)
    get_target_property(target_type ${target} TYPE)
    get_target_property(shader_sources_rel ${target} realproject_glsl_sources_rel)
    get_target_property(shader_headers_rel ${target} realproject_glsl_headers_rel)
    list(APPEND all_sources ${shader_sources_rel})
    list(APPEND all_sources ${shader_headers_rel})
    list(LENGTH all_sources shader_count)
    if(${target_type} STREQUAL "INTERFACE_LIBRARY")
        if(${shader_count} GREATER 0)
            message(FATAL_ERROR "Target ${target}: Interface libraries cannot have shaders")
        endif()
        set(target_has_shaders NO PARENT_SCOPE)
    elseif(${shader_count} EQUAL 0)
        set(target_has_shaders NO PARENT_SCOPE)
    else()
        set(target_has_shaders YES PARENT_SCOPE)
    endif()
endfunction()

function(_parse_string_constant_from_cpp cpp_code var_name)
    string(REGEX MATCH "${var_name} = \"([^\"]*)\"" _ ${cpp_code})
    set("${var_name}_VAL" "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

function(_load_shader_naming_constants)
    file(READ
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../src/RealEngine/utility/details/CMakeConstants.hpp"
        header_file
    )
    _parse_string_constant_from_cpp("${header_file}" k_shaderTargetSuffix)
    set(shader_target_suffix ${k_shaderTargetSuffix_VAL} PARENT_SCOPE)
    _parse_string_constant_from_cpp("${header_file}" k_shaderSPIRVCFileExt)
    set(shader_c_ext ${k_shaderSPIRVCFileExt_VAL} PARENT_SCOPE)
    _parse_string_constant_from_cpp("${header_file}" k_shaderDepFileExt)
    set(shader_dep_ext ${k_shaderDepFileExt_VAL} PARENT_SCOPE)
    _parse_string_constant_from_cpp("${header_file}" k_shaderSPIRVDisFileExt)
    set(shader_txt_ext ${k_shaderSPIRVDisFileExt_VAL} PARENT_SCOPE)
    _parse_string_constant_from_cpp("${header_file}" k_shaderSPIRVBinFileExt)
    set(shader_bin_ext ${k_shaderSPIRVBinFileExt_VAL} PARENT_SCOPE)
endfunction()