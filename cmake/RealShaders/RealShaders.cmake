# author     Dubsky Tomas

include("cmake/RealShaders/CollateShaders.cmake")
include("cmake/RealShaders/GenerateCppFiles.cmake")

# Adds library that can also have shaders added via 'target_shaders'
macro(add_library_with_shaders)
    add_library(${ARGV})
    set_target_properties(${ARGV0} PROPERTIES REALSHADERS_GLSL_STANDARD "460")
    cmake_language(DEFER CALL RealShaders_CollateShaders ${ARGV0})
endmacro()

# Adds executable that can also have shaders added via 'target_shaders'
macro(add_executable_with_shaders)
    add_executable(${ARGV})
    set_target_properties(${ARGV0} PROPERTIES REALSHADERS_GLSL_STANDARD "460")
    cmake_language(DEFER CALL RealShaders_CollateShaders ${ARGV0})
endmacro()

# Adds shader source files to a target
function(target_shaders target scope)
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    file(RELATIVE_PATH path_rel ${target_source_dir} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach (shader_source IN LISTS ARGN)
        set_property(TARGET ${target}
                        APPEND PROPERTY REALSHADERS_SOURCES_REL "${path_rel}/${shader_source}")
        set_property(TARGET ${target}
                        APPEND PROPERTY REALSHADERS_SOURCES_ABS "${CMAKE_CURRENT_SOURCE_DIR}/${shader_source}")
    endforeach()
    target_sources(${target} ${scope} ${ARGN})
    RealShaders_GenerateCppFiles(${target} ${scope} ${path_rel} ${ARGN})
endfunction()

# Adds C++ sources and shader source files to a target
function(target_sources_and_shaders target scope)
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    file(RELATIVE_PATH path_rel ${target_source_dir} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach (source IN LISTS ARGN)
        get_filename_component(source_ext ${source} LAST_EXT)
        if (NOT ("${source_ext}" STREQUAL ".hpp" OR "${source_ext}" STREQUAL ".cpp"))
            set_property(TARGET ${target}
                APPEND PROPERTY REALSHADERS_SOURCES_REL "${path_rel}/${source}")
            set_property(TARGET ${target}
                APPEND PROPERTY REALSHADERS_SOURCES_ABS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
            list(APPEND shaders ${source})
        endif()
    endforeach()
    target_sources(${target} ${scope} ${ARGN})
    if (DEFINED shaders)
        RealShaders_GenerateCppFiles(${target} ${scope} ${path_rel} ${shaders})
    endif()
endfunction()