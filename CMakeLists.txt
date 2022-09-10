cmake_minimum_required(VERSION 3.23)
project(RealEngine)

# std library
# Temporary solution until the std-as-module is standardized.
add_library(std_as_module STATIC)
set_target_properties(std_as_module PROPERTIES
    CXX_STANDARD 20
)

# RealEngine library
add_library(RealEngine STATIC)
set_target_properties(RealEngine PROPERTIES
    CXX_STANDARD 20
)

# OpenGL
find_package(OpenGL REQUIRED)

# GLEW
find_package(GLEW 2.0 REQUIRED)

# SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)

# GLM
include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
# GLM as a C++20 module
add_library(glm_as_module STATIC)
set_target_properties(glm_as_module PROPERTIES
    CXX_STANDARD 20
)
target_link_libraries(glm_as_module
    PUBLIC
        glm::glm
)

# nlohmann json
set(JSON_Install ON)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.10.5
)

FetchContent_MakeAvailable(glm json)

target_link_libraries(RealEngine
    PUBLIC
        std_as_module
        ${CMAKE_DL_LIBS}
        OpenGL::GL
        GLEW::GLEW
        SDL2::Main
        glm_as_module
        nlohmann_json::nlohmann_json
)

target_include_directories(RealEngine
    PUBLIC
        .
        external
)

# RTICreator  (Windows only)
if(WIN32)
    add_executable(RTICreator)
    target_include_directories(RTICreator PRIVATE .)

    set_target_properties(RTICreator PROPERTIES
        CXX_STANDARD 20
    )

    target_link_libraries(RTICreator
        PUBLIC
            RealEngine
    )
	
    add_subdirectory(RTICreator)
endif()

add_subdirectory(external)
add_subdirectory(RealEngine)