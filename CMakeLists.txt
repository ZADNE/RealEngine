cmake_minimum_required(VERSION 3.14)
project(RealEngine)

#Vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc)
#OpenGL
find_package(OpenGL REQUIRED)
#GLEW
find_package(GLEW 2.0 REQUIRED)
#SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)
#GLM
include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
#nlohmann json
set(JSON_Install ON)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.9.1
)
FetchContent_MakeAvailable(glm json)

#RealEngine library
add_library(RealEngine STATIC)
set_target_properties(RealEngine PROPERTIES
    CXX_STANDARD 20
)
target_link_libraries(RealEngine
    PUBLIC
        ${CMAKE_DL_LIBS}
        Vulkan::Vulkan
        OpenGL::GL
        GLEW::GLEW
        SDL2::Main
        glm
        nlohmann_json::nlohmann_json
)
target_include_directories(RealEngine
    PUBLIC
        .
        external
        ${CMAKE_CURRENT_BINARY_DIR}/spir_v
)
#BasicShaders
file(GLOB_RECURSE SHADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.vert *.frag )
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(GLSLC_FLAGS "-O")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(GLSLC_FLAGS "-g")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    set(GLSLC_FLAGS "-Os")
else()
    set(GLSLC_FLAGS "-O0" "-g")
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message(WARNING "Configuting unknown configuration (not Release, Debug, RelWithDebInfo or MinSizeRel). GLSL will be compiled as Debug.")
    endif()
endif()
foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(SHADER_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/spir_v/${SHADER} DIRECTORY)
    file(MAKE_DIRECTORY ${SHADER_BIN_DIR})
    set(SHADER_SPIRV ${CMAKE_CURRENT_BINARY_DIR}/spir_v/${SHADER}.spv)
    add_custom_command(OUTPUT ${SHADER_SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${SHADER_SPIRV} -mfmt=c ${GLSLC_FLAGS}
        DEPENDS ${SHADER}
    )
    list(APPEND SHADERS_SPIRV ${SHADER_SPIRV})
endForeach()
add_custom_target(BasicShaders ALL DEPENDS ${SHADERS_SPIRV})
add_dependencies(RealEngine BasicShaders)
#RTICreator (Windows only)
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

add_subdirectory(RealEngine)
add_subdirectory(external)
