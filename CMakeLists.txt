cmake_minimum_required(VERSION 3.24)
project(RealEngine)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

include("cmake/GetRealEngineVersion.cmake")

function(FetchContent_MakeAvailableWithoutLinting)
    set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
    set(CMAKE_CXX_CLANG_TIDY "")
    FetchContent_MakeAvailable(${ARGV})
endfunction()

# Find/fetch dependencies
    # Vulkan
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    # SDL2
    find_package(SDL2 CONFIG REQUIRED GLOBAL)
    find_package(SDL2_ttf CONFIG REQUIRED GLOBAL)
    # SPIRV-Cross
    FetchContent_Declare(SPIRV-Cross URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.3.290.0.tar.gz)
    # OpenGL Mathematics
    set(GLM_BUILD_INSTALL ON CACHE INTERNAL "")
    FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/1.0.0.tar.gz)
    # nlohmann json
    set(JSON_Install ON CACHE INTERNAL "")
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

    # Download remote libs
    FetchContent_MakeAvailableWithoutLinting(SPIRV-Cross glm json)

    # Add embedded libs
    add_subdirectory(external)

# Declare projects
    # RealEngine library
    add_real_library(RealEngine STATIC)
    set_target_properties(RealEngine PROPERTIES
        CXX_STANDARD 23
        realproject_shader_cxx_namespace "re::glsl"
        INTERPROCEDURAL_OPTIMIZATION TRUE
    )
    target_link_libraries(RealEngine
        PUBLIC
            ${CMAKE_DL_LIBS}
            Vulkan::Vulkan
            vma
            SDL2::SDL2main
            SDL2::SDL2
            SDL2_ttf::SDL2_ttf
            glm::glm-header-only
            nlohmann_json::nlohmann_json
            ImGui
            lodepng
        PRIVATE
            spirv-cross-core
            spirv-cross-reflect
    )
    target_include_directories(RealEngine
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:.>
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external>
    )
    real_target_init_file_sets(RealEngine BASE_DIR src)
    get_realengine_version()
    add_subdirectory(src)

    # Doxygen documentation
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(RealEngine_Documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM
        )
    endif(DOXYGEN_FOUND)

    # RTICreator (Windows only)
    if (WIN32)
        add_real_executable(RTICreator)
        target_include_directories(RTICreator PRIVATE tools)
        set_target_properties(RTICreator PROPERTIES
            CXX_STANDARD 23
            INTERPROCEDURAL_OPTIMIZATION TRUE
        )
        target_link_libraries(RTICreator PUBLIC RealEngine)
        set(RTICreator "RTICreator")
    else()
        set(RTICreator "")
    endif()
    add_subdirectory(tools)

# Installation
    install(TARGETS RealEngine vma glm-header-only nlohmann_json ImGui lodepng ${RTICreator}
        EXPORT RealEngineTargets
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT           RealEngine_Runtime
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT           RealEngine_Runtime
            NAMELINK_COMPONENT  RealEngine_Development
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT           RealEngine_Development
        FILE_SET realproject_public_headers
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT           RealEngine_Development
        FILE_SET HEADERS
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT           RealEngine_Development
    )
    write_basic_package_version_file(
        RealEngineConfigVersion.cmake
        VERSION ${realengine_version}
        COMPATIBILITY SameMajorVersion
    )
    install(EXPORT RealEngineTargets
        FILE RealEngineTargets.cmake
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/RealEngine/cmake"
    )
    configure_file(cmake/RealEngineConfig.cmake.in RealEngineConfig.cmake @ONLY)
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfigVersion.cmake
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/RealEngine/cmake"
    )