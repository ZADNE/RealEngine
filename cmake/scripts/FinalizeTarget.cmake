# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/GenerateCppModuleInterface.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GenerateSetupFile.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/AddShaderCompilationTarget.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GenerateShaderWrappers.cmake")

function(_re_finalize_real_target target)
    _re_generate_cpp_setup_file(${target})
    _re_generate_shaders_wrappers(${target})
    _re_generate_cpp_interface_module(${target})
    _re_add_shader_compilation_target(${target})
endfunction()
