# author    Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/GenerateCppModuleInterface.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GenerateSetupFile.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/AddShaderCompilationTarget.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/GenerateShaderWrappers.cmake")

function(_finalize_real_target target)
    _generate_cpp_setup_file(${target})
    _generate_shaders_wrappers(${target})
    _generate_cpp_interface_module(${target})
    _add_shader_compilation_target(${target})
endfunction()
