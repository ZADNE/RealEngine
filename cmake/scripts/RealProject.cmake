# author     Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/RealProjectSourceProcessing.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/RealProjectGenerateSetupFile.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/RealShadersAddCompilationTarget.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/RealShadersGenerateCppFiles.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ResourcePackaging.cmake")

# Do not use directly, use add_real_library or add_real_executable
macro(_add_real_target target)
    get_target_property(source_dir ${target} SOURCE_DIR)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD                            23
        CXX_MODULE_STD                          ON
        realproject_sources_as_cxx_impl_units   OFF
        realproject_base_dir_rel                "."
        realproject_base_dir_abs                "${source_dir}"
        realproject_setup_filepath_rel          "${target}Setup.gen.hpp"
        realproject_glsl_standard               "460"
        realproject_glsl_headers_rel            ""
        realproject_glsl_sources_rel            ""
    )
    cmake_language(DEFER CALL _generate_cpp_setup_file ${target})
    cmake_language(DEFER CALL _generate_cpp_wrappers_for_shaders ${target})
    cmake_language(DEFER CALL _add_shader_compilation_target ${target})
endmacro()

# Adds a Real library
# Add both C++ and GLSL sources via real_target_sources
macro(add_real_library)
    add_library(${ARGV})
    _add_real_target(${ARGV0})
endmacro()

# Adds a Real library
# Add both C++ and GLSL sources via real_target_sources
macro(add_real_executable)
    add_executable(${ARGV})
    _add_real_target(${ARGV0})
    # Copy DLLs in post build step
    add_custom_command(TARGET ${ARGV0} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
            copy -t "$<TARGET_FILE_DIR:${ARGV0}>" "$<TARGET_RUNTIME_DLLS:${ARGV0}>"
        COMMAND_EXPAND_LISTS
    )
endmacro()
