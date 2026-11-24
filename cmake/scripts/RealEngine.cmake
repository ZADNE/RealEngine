# author     Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Setup.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SourceProcessing.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/FinalizeTarget.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ResourcePackaging.cmake")

# Do not use directly, use add_real_library or add_real_executable
macro(_add_real_target target)
    get_target_property(source_dir ${target} SOURCE_DIR)
    set_target_properties(${target} PROPERTIES
        RE_CXX_INTERFACE_MODULE     ""
        RE_BASE_DIR_REL             "."
        RE_BASE_DIR_ABS             "${source_dir}"
        RE_SETUP_FILEPATH_REL       ""
        RE_SETUP_MODULE             ""
        RE_GLSL_HEADERS_REL         ""
        RE_GLSL_SOURCES_REL         ""
        RE_IS_REAL_TARGET           ON
        ${RE_REAL_TARGET_DEFAULT_PROPERTY_OVERRIDES}
    )
    cmake_language(DEFER CALL _finalize_real_target ${target})
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
