# author     Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

# Create a custom target that will index and package files:
# Syntax:
#     re_target_package_resources(
#        TARGET <target>
#        PACKAGE_DIR <directory_name>
#        [ INDEX_FILE <file_path> ]
#        [ MODULE_NAME <module_name> ]
#        INPUT_DIRS <directory_name>...
#     )
# The index is a C++ header file containing re::ResourceIDs of the packaged files.
# The target will be named <target>_PackageResources.
# The packaging will be done in non-debug builds only as debug build of RealEngine
# reads the unpackaged data.
function(re_target_package_resources)
    set(one_value_args TARGET PACKAGE_DIR INDEX_FILE MODULE_NAME)
    set(multi_value_args INPUT_DIRS)
    cmake_parse_arguments(ARG "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    # Argument validation
    if(NOT DEFINED ARG_TARGET)
        message(FATAL_ERROR "Name of target to package resources for not provided.")
    endif()
    if(NOT DEFINED ARG_INDEX_FILE)
        get_target_property(base_dir ${ARG_TARGET} RE_BASE_DIR_REL)
        set(ARG_INDEX_FILE
            "${CMAKE_CURRENT_BINARY_DIR}/${base_dir}/${ARG_TARGET}/ResourceIndex.gen.cppm"
        )
    endif()
    if(DEFINED ARG_MODULE_NAME)
        set(module_name ${ARG_MODULE_NAME})
    else()
        string(TOLOWER ${ARG_TARGET} target_lower)
        set(module_name "${target_lower}:ResourceIndex_gen")
    endif()
    set(output_package "${ARG_PACKAGE_DIR}/package.dat")

    # Copy dummy index which will be used for debug builds
    if(NOT EXISTS ${ARG_INDEX_FILE})
        configure_file(
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cpp_templates/ResourceIndex.cppm.in"
            ${ARG_INDEX_FILE}
            @ONLY
        )
    endif()

    # Make the index file part of target
    target_sources(${ARG_TARGET}
        PUBLIC
            FILE_SET re_public_module_interfaces
            FILES ${ARG_INDEX_FILE}
    )

    # Copy 7z.dll to the target so that it can open the package
    re_copy_7z_dll_to_target(${ARG_TARGET})

    # Add the packaging command
    add_custom_command(
        OUTPUT ${output_package}
        COMMAND ResourcePackager
                    "$<LIST:TRANSFORM,${ARG_INPUT_DIRS},PREPEND,--in=>"
                    -o ${ARG_PACKAGE_DIR}
                    --index ${ARG_INDEX_FILE}
                    --module_name ${module_name}
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

