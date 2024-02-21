cmake_minimum_required(VERSION 3.24)
project(RealEngine)

include(FetchContent)
include("cmake/RealShaders/RealShaders.cmake")

#Find/fetch dependencies
    #Vulkan
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    #SDL2
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
    find_package(SDL2 REQUIRED)
    #SPIRV-Cross
    FetchContent_Declare(SPIRV-Cross URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.3.275.0.tar.gz)
    #OpenGL Mathematics
    FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/1.0.0.tar.gz)
    #nlohmann json
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

    #Download remote libs
    FetchContent_MakeAvailable(SPIRV-Cross glm json)

    #Add embedded libs
    add_subdirectory(external)

#Declare projects
    #RealEngine library
    add_library_with_shaders(RealEngine STATIC)
    set_target_properties(RealEngine PROPERTIES
        CXX_STANDARD 23
        REALSHADERS_CXX_NAMESPACE "re"
        INTERPROCEDURAL_OPTIMIZATION TRUE
    )
    target_link_libraries(RealEngine PUBLIC
        ${CMAKE_DL_LIBS}
        Vulkan::Vulkan
        vma
        spirv-cross-core
        spirv-cross-reflect
        SDL2::Main
        glm
        nlohmann_json::nlohmann_json
        ImGui
        lodepng
    )
    target_include_directories(RealEngine PUBLIC src external)
    add_subdirectory(src)

    #Doxygen documentation
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(RealEngine_Documentation ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM
        )
    endif(DOXYGEN_FOUND)

    #RTICreator (Windows only)
    if(WIN32)
        add_executable(RTICreator)
        target_include_directories(RTICreator PRIVATE tools)
        set_target_properties(RTICreator PROPERTIES
            CXX_STANDARD 23
            INTERPROCEDURAL_OPTIMIZATION TRUE
        )
        target_link_libraries(RTICreator PUBLIC RealEngine)
        add_subdirectory(tools)
    endif()