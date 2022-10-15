#author     Dubsky Tomas

include("cmake/RealShaders/CollateShaders.cmake")
include("cmake/RealShaders/GenerateCppFiles.cmake")

#Adds library that can also have shaders added via 'target_shaders'
macro(add_library_with_shaders)
    add_library(${ARGV})
    set_target_properties(${ARGV0} PROPERTIES GLSL_STANDARD "460")
    cmake_language(DEFER CALL RealShaders_collateShaders ${ARGV0})
endmacro()

#Adds executable that can also have shaders added via 'target_shaders'
macro(add_executable_with_shaders)
    add_executable(${ARGV})
    set_target_properties(${ARGV0} PROPERTIES GLSL_STANDARD "460")
    cmake_language(DEFER CALL RealShaders_collateShaders ${ARGV0})
endmacro()

#Adds shader source files to a target
function(target_shaders target)
    file(RELATIVE_PATH path_rel ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    foreach (shader_source IN LISTS ARGN)
        set_property(TARGET ${target}
                        APPEND PROPERTY SHADER_SOURCES_REL "${path_rel}/${shader_source}")
        set_property(TARGET ${target}
                        APPEND PROPERTY SHADER_SOURCES_ABS "${CMAKE_CURRENT_SOURCE_DIR}/${shader_source}")
    endforeach()
    target_sources(${target} PRIVATE ${ARGN})
    RealShaders_GenerateCppFiles(${target} ${path_rel} ${ARGN})
endfunction()