# author     Dubsky Tomas

function(add_include_path_without_suffix target scope suffixed_path expected_suffix)
    cmake_path(GET suffixed_path FILENAME path_suffix)
    if ("${path_suffix}" STREQUAL "${expected_suffix}")
        cmake_path(REMOVE_FILENAME suffixed_path
                   OUTPUT_VARIABLE path_without_suffix
        )
    target_include_directories(${target} ${scope} ${path_without_suffix})
    endif()
endfunction()

# A silly hack for SDL2 to be included as <SDL2/SDL.h> 
function(target_includes_sdl2_prefixed target scope)
    add_include_path_without_suffix(${target} ${scope} ${SDL2_INCLUDE_DIRS} "SDL2")
    add_include_path_without_suffix(${target} ${scope} ${SDL2_TTF_INCLUDE_DIRS} "SDL2")
endfunction()