# author     Dubsky Tomas

# Create a custom target that will index and package files:
# Syntax:
#     real_target_package_resources(
#        TARGET <target>
#        PACKAGE_DIR <directory_name>
#        [ INDEX_FILE <file_path> ]
#        INPUT_DIRS <directory_name>...
#     )
# The index is a C++ header file containing re::ResourceIDs of the packaged files.
# The target will be named <target>_PackageResources.
# The packaging will be done in non-debug builds only as debug build of RealEngine
# reads the unpackaged data.
function(real_target_package_resources)
    set(one_value_args TARGET PACKAGE_DIR INDEX_FILE)
    set(multi_value_args INPUT_DIRS)
    cmake_parse_arguments(ARG "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT DEFINED ARG_INDEX_FILE)
        get_target_property(base_dir ${ARG_TARGET} realproject_base_dir_rel)
        set(ARG_INDEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${ARG_TARGET}/ResourceIndex.gen.hpp")
    endif()
    set(output_package "${ARG_PACKAGE_DIR}/package.dat")
    get_target_property(realengine_source_dir RealEngine HEADER_DIRS_realproject_public_headers)

    # Copy dummy index which will be used for debug builds
    if(NOT EXISTS ${ARG_INDEX_FILE})
        configure_file(
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates/ResourceIndex.hpp.in"
            ${ARG_INDEX_FILE}
            COPYONLY
        )
    endif()

    # Add the packaging command
    add_custom_command(
        OUTPUT ${output_package}
        COMMAND ResourcePackager
                    "$<LIST:TRANSFORM,${ARG_INPUT_DIRS},PREPEND,--in=>"
                    -o ${ARG_PACKAGE_DIR}
                    --index ${ARG_INDEX_FILE}
        DEPENDS ResourcePackager
        BYPRODUCTS ${ARG_INDEX_FILE}
        COMMENT "Packaging resources for ${ARG_TARGET}..."
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    set(package_target "${ARG_TARGET}_PackageResources")
    add_custom_target(${package_target} DEPENDS $<$<NOT:$<CONFIG:Debug>>:${output_package}>)
    add_dependencies(${ARG_TARGET} ${package_target})
endfunction()

