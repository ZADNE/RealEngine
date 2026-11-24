# author     Dubsky Tomas

# Initializes file sets of a real target
# Syntax:
#     real_target_init_file_sets(
#        <target>
#        BASE_DIR <directory_name>
#        [ GENERATE_CXX_MODULE_INTERFACE <module_name> ]
#     )
# BASE_DIR <directory_name>
#     Header includes will be relative to this directory.
# GENERATE_CXX_MODULE_INTERFACE <module_name>
#     A C++ module interface will be generated.
#     All target's module partition interfaces must declare partition as:
#         export module <module_name>:folder.subfolder.FileName;
#     The module interface will not be valid if this condition is not satisfied.
function(real_target_init_file_sets target)
    set(option_keywords "")
    set(one_value_keywords BASE_DIR GENERATE_CXX_MODULE_INTERFACE)
    cmake_parse_arguments(ARG "${option_keywords}" "${one_value_keywords}" "" ${ARGN})

    get_target_property(source_dir ${target} SOURCE_DIR)
    set_target_properties(${target} PROPERTIES
        RE_BASE_DIR_REL "${ARG_BASE_DIR}"
        RE_BASE_DIR_ABS "${source_dir}/${ARG_BASE_DIR}"
    )
    target_sources(${target} PUBLIC FILE_SET re_public_headers
        TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_BASE_DIR}"
    )

    if(DEFINED ARG_GENERATE_CXX_MODULE_INTERFACE)
        set_target_properties(${target} PROPERTIES
            RE_CXX_INTERFACE_MODULE "${ARG_GENERATE_CXX_MODULE_INTERFACE}"
        )
    endif()

    # Module file sets
    get_target_property(target_type ${target} TYPE)
    if(NOT (${target_type} STREQUAL "INTERFACE_LIBRARY"))
        target_sources(${target}
            PUBLIC
                FILE_SET re_public_module_interfaces
                TYPE CXX_MODULES
                BASE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_BASE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/${ARG_BASE_DIR}"
            PRIVATE
                FILE_SET re_private_module_interfaces
                TYPE CXX_MODULES
                BASE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_BASE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/${ARG_BASE_DIR}"
            PRIVATE
                FILE_SET re_module_impl_units
                TYPE CXX_MODULES
                BASE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_BASE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/${ARG_BASE_DIR}"
        )
    endif()
endfunction()

# Based on file extensions, add source files into the target
function(_incorporate_scoped_sources target path_rel file_scope)
    get_target_property(sources_as_cxx_impl_units ${target} RE_SOURCES_AS_CXX_IMPL_UNITS)
    foreach(source IN LISTS ARGN)
        get_filename_component(source_ext ${source} LAST_EXT)
        if(${source_ext} IN_LIST RE_MODULE_INTERFACE_EXTENSIONS)
            if(${file_scope} STREQUAL "PUBLIC")
                target_sources(${target}
                    PUBLIC
                        FILE_SET re_public_module_interfaces
                        FILES ${source}
                )
            elseif(${file_scope} STREQUAL "PRIVATE")
                target_sources(${target}
                    PRIVATE
                        FILE_SET re_private_module_interfaces
                        FILES ${source}
                )
            endif()
        elseif(${source_ext} IN_LIST RE_HEADER_EXTENSIONS)
            if(${file_scope} STREQUAL "PUBLIC")
                target_sources(${target}
                    PUBLIC
                        FILE_SET re_public_headers
                        FILES ${source}
                )
            elseif(${file_scope} STREQUAL "PRIVATE")
                target_sources(${target} PRIVATE ${source})
            endif()
        elseif(${source_ext} IN_LIST RE_SOURCE_EXTENSIONS)
            if(${sources_as_cxx_impl_units})
                target_sources(${target}
                    PRIVATE
                        FILE_SET re_module_impl_units
                        FILES ${source}
                )
            else()
                target_sources(${target} PRIVATE ${source})
            endif()
        elseif(${source_ext} IN_LIST RE_GLSL_HEADER_EXTENSIONS)
            set_property(TARGET ${target}
                APPEND PROPERTY RE_GLSL_HEADERS_REL
                "${path_rel}/${source}"
            )
        elseif(${source_ext} IN_LIST RE_GLSL_STAGE_EXTENSIONS)
            set_property(TARGET ${target}
                APPEND PROPERTY RE_GLSL_SOURCES_REL
                "${path_rel}/${source}"
            )
        else()
            message(FATAL_ERROR
                "Target ${target}: ${path_rel}/${source} has unknown file extension."
            )
        endif()
    endforeach()
endfunction()

# Adds C++ source files (headers, source, shaders) files to a target
# C++ headers maintain the scope (PUBLIC, PRIVATE)
# C++ sources and GLSL headers/stages are always private
function(real_target_sources target)
    # Sanity check
    get_target_property(is_real_target ${target} RE_IS_REAL_TARGET)
    if(NOT ${is_real_target})
        message(FATAL_ERROR
            "Target ${target} is not a Real target, "
            "and thus real_target_sources cannot be used on it."
        )
        return()
    endif()

    # Separate sources into public and private categories
    set(multi_value_args "PUBLIC" "PRIVATE")
    cmake_parse_arguments(ARG "" "" "${multi_value_args}" ${ARGN})

    # Get relative path
    get_target_property(target_base_dir ${target} RE_BASE_DIR_ABS)
    cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR
        BASE_DIRECTORY ${target_base_dir} OUTPUT_VARIABLE path_rel
    )

    # Incorporate the sources
    _incorporate_scoped_sources(${target} ${path_rel} PUBLIC ${ARG_PUBLIC})
    _incorporate_scoped_sources(${target} ${path_rel} PRIVATE ${ARG_PRIVATE})
endfunction()
