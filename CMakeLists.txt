cmake_minimum_required(VERSION 3.24)
project(RealEngine)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

set(REALENGINE_BUILDING_FROM_SOURCE TRUE
    CACHE INTERNAL "Whether building RealEngine from source (as opposed to using installed)"
)

include("cmake/scripts/RealProject.cmake")
include("cmake/scripts/GetRealEngineVersion.cmake")

function(FetchContent_MakeAvailableWithoutLinting)
    set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
    set(CMAKE_CXX_CLANG_TIDY "")
    FetchContent_MakeAvailable(${ARGV})
endfunction()

# Find/fetch dependencies
    # Git is required to download some dependencies
    find_package(Git REQUIRED)
    # Vulkan
    find_package(Vulkan REQUIRED COMPONENTS glslc)
    # SDL2
    find_package(SDL2 CONFIG REQUIRED GLOBAL)
    find_package(SDL2_ttf CONFIG REQUIRED GLOBAL)
    # SPIRV-Cross
    FetchContent_Declare(SPIRV-Cross
        URL https://github.com/KhronosGroup/SPIRV-Cross/archive/refs/tags/vulkan-sdk-1.3.290.0.tar.gz
    )
    # OpenGL Mathematics
    set(GLM_BUILD_INSTALL ON CACHE INTERNAL "")
    FetchContent_Declare(glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz
    )
    # bit7z
    set(BIT7Z_7ZIP_VERSION "23.01" CACHE INTERNAL "")
    FetchContent_Declare(bit7z
        URL https://github.com/rikyoz/bit7z/archive/refs/tags/v4.0.9.zip
    )
    # nlohmann json
    set(JSON_Install ON CACHE INTERNAL "")
    FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )

    # Make all imported libs implicitly install as a part of RealEngine_Development
    set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME RealEngine_Development)

    # Download remote libs
    FetchContent_MakeAvailableWithoutLinting(SPIRV-Cross glm bit7z json)

    # Create imported target for 7z.dll
    if(WIN32)
        add_library(7z SHARED IMPORTED GLOBAL)
        set_target_properties(7z PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/external/bin/win_x64/7z.dll"
        )
        list(APPEND runtime_bundled_artifacts 7z)
    endif()

    # Remove interface headers with absolute paths from bit7z target
    get_target_property(bit7z_aliased_target bit7z ALIASED_TARGET)
    if(NOT TARGET ${bit7z_aliased_target})
        set(bit7z_aliased_target bit7z)
    endif()
    set_target_properties(${bit7z_aliased_target}
        PROPERTIES
            INTERFACE_SOURCES ""
    )

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
            bit7z
            nlohmann_json::nlohmann_json
            ImGui
            lodepng
        PRIVATE
            spirv-cross-core
            spirv-cross-reflect
            filewatch
    )
    target_include_directories(RealEngine
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
            "$<INSTALL_INTERFACE:.>"
    )
    real_target_init_file_sets(RealEngine BASE_DIR "src")
    get_realengine_version()
    add_subdirectory("src")

    # Doxygen documentation
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
        set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
        configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

        add_custom_target(RealEngine_Documentation
            COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            VERBATIM
        )
    endif(DOXYGEN_FOUND)

    # ResourcePackager
    add_real_executable(ResourcePackager)
    target_include_directories(ResourcePackager
        PRIVATE
            "src"
            "tools"
    )
    set_target_properties(ResourcePackager PROPERTIES
        CXX_STANDARD 23
        INTERPROCEDURAL_OPTIMIZATION TRUE
    )
    target_link_libraries(ResourcePackager
        PRIVATE
            argparse
            bit7z
    )

    # RTICreator (Windows only)
    if (WIN32)
        add_real_executable(RTICreator)
        target_include_directories(RTICreator PRIVATE "tools")
        set_target_properties(RTICreator PROPERTIES
            CXX_STANDARD 23
            INTERPROCEDURAL_OPTIMIZATION TRUE
        )
        target_link_libraries(RTICreator PRIVATE RealEngine)
        set(RTICreator "RTICreator")
    else()
        set(RTICreator "")
    endif()
    add_subdirectory("tools")

# Installation
    # The main install command
    install(
        TARGETS
            RealEngine vma glm-header-only ${bit7z_aliased_target}
            nlohmann_json ImGui lodepng argparse filewatch
            ResourcePackager ${RTICreator}
        EXPORT                  RealEngineTargets
        RUNTIME # Tool executables are part of RealEngine development
            DESTINATION         "${CMAKE_INSTALL_BINDIR}"
            COMPONENT           RealEngine_Development
        LIBRARY
            DESTINATION         "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT           RealEngine_Development
            NAMELINK_COMPONENT  RealEngine_Development
        ARCHIVE
            DESTINATION         "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT           RealEngine_Development
        FILE_SET realproject_public_headers
            DESTINATION         "${CMAKE_INSTALL_INCLUDEDIR}"
            COMPONENT           RealEngine_Development
        FILE_SET HEADERS
            DESTINATION         "${CMAKE_INSTALL_INCLUDEDIR}"
            COMPONENT           RealEngine_Development
    )
    install( # Manually install RealShaders because it contains GLSL that normally is not installed
        DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/src/RealShaders"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT   RealEngine_Development
    )

    # Manually install headers and necessary scripts for bit7z
    # because it does not have install rules
    install(
        DIRECTORY   "${FETCHCONTENT_BASE_DIR}/bit7z-src/include/bit7z"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT   RealEngine_Development
    )
    install(
        FILES
            "${FETCHCONTENT_BASE_DIR}/bit7z-src/cmake/FilesystemDetect.cmake"
            "${FETCHCONTENT_BASE_DIR}/bit7z-src/cmake/BuildOptions.cmake"
            "${FETCHCONTENT_BASE_DIR}/bit7z-src/cmake/Dependencies.cmake"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/bit7z/cmake"
        COMPONENT   RealEngine_Development
    )

    # Install bundled precompiled dlls of external projects
    install(
        IMPORTED_RUNTIME_ARTIFACTS ${runtime_bundled_artifacts} # SDLs are not bundled
        RUNTIME
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT   RealEngine_Development
    )

    # Downstream projects should install these runtime artifacts from these targets
    set(REALENGINE_RUNTIME_ARTIFACTS 
        SDL2::SDL2 SDL2_ttf::SDL2_ttf ${runtime_bundled_artifacts}
        CACHE INTERNAL ""
    )

    # Install documentation
    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/doc/")
        install(
            DIRECTORY   "${CMAKE_CURRENT_BINARY_DIR}/doc/"
            DESTINATION "${CMAKE_INSTALL_DOCDIR}"
            COMPONENT   RealEngine_Development
        )
    endif()

    # Configure/generate and install CMake scripts
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfigVersion.cmake"
        VERSION ${realengine_version}
        COMPATIBILITY SameMajorVersion
    )
    install(
        EXPORT RealEngineTargets
        FILE "RealEngineTargets.cmake"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/RealEngine/cmake"
        COMPONENT RealEngine_Development
    )
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_templates/RealEngineConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfig.cmake"
        @ONLY
    )
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/RealEngineConfigVersion.cmake"
        DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/RealEngine/cmake"
        COMPONENT RealEngine_Development
    )
    install( # RealProject scripts and C++ template files
        DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp_templates"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts"
        DESTINATION
            "${CMAKE_INSTALL_DATAROOTDIR}/RealEngine/cmake"
        COMPONENT RealEngine_Development
    )