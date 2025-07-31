# author     Dubsky Tomas

function(get_realengine_version building_from_source)
    if(building_from_source)
        set(realengine_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../src")
    else()
        get_target_property(base_dir RealEngine BINARY_DIR)
        set(realengine_dir "${base_dir}/src")
    endif()
    file(READ
        "${realengine_dir}/RealEngine/utility/Version.hpp"
        version_file
    )
    string(REGEX MATCH "constexpr int k_versionMajor = ([0-9]+)" _ ${version_file})
    set(version_major "${CMAKE_MATCH_1}")
    string(REGEX MATCH "constexpr int k_versionMinor = ([0-9]+)" _ ${version_file})
    set(version_minor "${CMAKE_MATCH_1}")
    string(REGEX MATCH "constexpr int k_versionPatch = ([0-9]+)" _ ${version_file})
    set(version_patch "${CMAKE_MATCH_1}")

    set(realengine_version "${version_major}.${version_minor}.${version_patch}" PARENT_SCOPE)
endfunction()

# Print the version to stdout if running in script mode
if(DEFINED CMAKE_ARGV0)
    get_realengine_version(TRUE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${realengine_version}")
endif()