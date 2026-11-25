# author     Dubsky Tomas

include("${CMAKE_CURRENT_LIST_DIR}/Utility.cmake")

function(re_get_engine_version out_var)
    re_get_engine_base_dir_abs(engine_base_dir_abs)
    file(READ
        "${engine_base_dir_abs}/RealEngine/utility/Version.cppm"
        version_file
    )
    string(REGEX MATCH "constexpr int k_versionMajor = ([0-9]+)" _ ${version_file})
    set(version_major "${CMAKE_MATCH_1}")
    string(REGEX MATCH "constexpr int k_versionMinor = ([0-9]+)" _ ${version_file})
    set(version_minor "${CMAKE_MATCH_1}")
    string(REGEX MATCH "constexpr int k_versionPatch = ([0-9]+)" _ ${version_file})
    set(version_patch "${CMAKE_MATCH_1}")

    set(${out_var} "${version_major}.${version_minor}.${version_patch}" PARENT_SCOPE)
endfunction()

# Print the version to stdout if running in script mode
if(DEFINED CMAKE_ARGV0)
    re_get_engine_version(engine_version)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${engine_version}")
endif()