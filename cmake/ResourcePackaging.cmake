# author     Dubsky Tomas

# Create a custom target that will index and package files:
# Syntax:
#     real_target_package_resources(
#        TARGET <target>
#        OUTPUT_DIR <directory_name>
#        INPUT_DIRS <directory_name>...
#     )
# The target will be named <target>_PackageResources.
# The packaging will be done in non-debug builds only as debug build of RealEngine
# reads the unpackaged data.
function(real_target_package_resources)
    set(one_value_args TARGET OUTPUT_DIR)
    set(multi_value_args INPUT_DIRS)
    cmake_parse_arguments(ARG "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(output_package "${ARG_OUTPUT_DIR}/package.dat")
    get_target_property(realengine_binary_dir RealEngine BINARY_DIR)

    add_custom_command(
        OUTPUT ${output_package}
        COMMAND ResourcePackager
                    "$<LIST:TRANSFORM,${ARG_INPUT_DIRS},PREPEND,--in=>"
                    -o ${ARG_OUTPUT_DIR}
                    --index "${realengine_binary_dir}/src/RealEngine/resources/ResourceIndex.hpp"
        DEPENDS ResourcePackager
        COMMENT "Packaging resources for ${ARG_TARGET}..."
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    set(package_target "${ARG_TARGET}_PackageResources")
    add_custom_target(${package_target} DEPENDS $<$<NOT:$<CONFIG:Debug>>:${output_package}>)
    add_dependencies(${ARG_TARGET} ${package_target})
endfunction()

