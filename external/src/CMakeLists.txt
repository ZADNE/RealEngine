# Disable linting for external targets
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(CMAKE_CXX_CLANG_TIDY "")
set(RE_SOURCES_AS_CXX_IMPL_UNITS OFF)

# argparse
re_add_library(argparse OBJECT)
re_target_init_file_sets(argparse BASE_DIR ".")
add_subdirectory(argparse)

# Filewatch
re_add_library(filewatch OBJECT)
re_target_init_file_sets(filewatch BASE_DIR ".")
add_subdirectory(filewatch)

# Dear ImGui
re_add_library(ImGui STATIC)
re_target_init_file_sets(ImGui BASE_DIR ".")
target_link_libraries(ImGui
    PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3
        glm::glm-header-only
)
add_subdirectory(imgui)

# lodepng
re_add_library(lodepng STATIC)
re_target_init_file_sets(lodepng BASE_DIR ".")
add_subdirectory(lodepng)

# Vulkan Memory Allocator
re_add_library(vma OBJECT)
re_target_init_file_sets(vma BASE_DIR ".")
target_link_libraries(vma
    PRIVATE
        Vulkan::HppModule
)
add_subdirectory(vma)
