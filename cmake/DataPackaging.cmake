# author     Dubsky Tomas

# Create a custom target that will package data:
# Syntax:
#     real_target_package_data(
#        TARGET <target>
#        OUTPUT_DIR <directory_name>
#        INPUT_DIRS <directory_name>...
#     )
function(real_target_package_data)
    set(one_value_args TARGET OUTPUT_DIR)
    set(multi_value_args INPUT_DIRS)
    cmake_parse_arguments(ARG "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(output_package "${ARG_OUTPUT_DIR}/package.dat")

    add_custom_command(
        OUTPUT ${output_package}
        COMMAND DataPackager "$<LIST:TRANSFORM,${ARG_INPUT_DIRS},PREPEND,--in=>"
                             -o ${ARG_OUTPUT_DIR}
        DEPENDS DataPackager
        COMMENT "Packaging data for ${ARG_TARGET}..."
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    set(package_target "${ARG_TARGET}_PackageData")
    add_custom_target(${package_target} DEPENDS ${output_package})
    add_dependencies(${ARG_TARGET} ${package_target})
endfunction()

