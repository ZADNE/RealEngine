cmake_minimum_required(VERSION 3.24)
project(RealEngine)

include(FetchContent)
include("cmake/RealShaders/RealShaders.cmake")

#Find/fetch dependencies
    #Vulkan
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    #OpenGL
    find_package(OpenGL REQUIRED)
    #GLEW
    find_package(GLEW 2.0 REQUIRED)
    #SDL2
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
    find_package(SDL2 REQUIRED)
    #SPIRV-Cross
    FetchContent_Declare(SPIRV-Cross URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/sdk-1.3.231.1.tar.gz)
    #GLM
    FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/0.9.9.8.tar.gz)
    #nlohmann json
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
    FetchContent_MakeAvailable(SPIRV-Cross glm json)

#Declare projects
    #RealEngine library
    add_library_with_shaders(RealEngine STATIC)
    set_target_properties(RealEngine PROPERTIES
        CXX_STANDARD 20
        SHADERS_CXX_NAMESPACE "RE"
    )
    target_link_libraries(RealEngine PUBLIC
        ${CMAKE_DL_LIBS}
        Vulkan::Vulkan spirv-cross-core spirv-cross-reflect
        OpenGL::GL GLEW::GLEW
        SDL2::Main
        glm
        nlohmann_json::nlohmann_json
    )
    target_include_directories(RealEngine PUBLIC . external)
    #RTICreator (Windows only)
    if(WIN32)
        add_executable(RTICreator)
        target_include_directories(RTICreator PRIVATE .)
        set_target_properties(RTICreator PROPERTIES CXX_STANDARD 20)
        target_link_libraries(RTICreator PUBLIC RealEngine)
        add_subdirectory(RTICreator)
    endif()

#Add subdirectories
    add_subdirectory(RealEngine)
    add_subdirectory(external)