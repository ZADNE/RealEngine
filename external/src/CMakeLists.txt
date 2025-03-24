# Disable linting for external targets
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(CMAKE_CXX_CLANG_TIDY "")

# argparse
add_real_library(argparse INTERFACE)
real_target_init_file_sets(argparse BASE_DIR ".")
add_subdirectory(argparse)

# Dear ImGui
add_real_library(ImGui STATIC)
real_target_init_file_sets(ImGui BASE_DIR ".")
target_link_libraries(ImGui
    PRIVATE
        Vulkan::Vulkan
        SDL2::SDL2
        glm::glm-header-only
)
add_subdirectory(ImGui)

# lodepng
add_real_library(lodepng STATIC)
real_target_init_file_sets(lodepng BASE_DIR ".")
add_subdirectory(lodepng)

# Vulkan Memory Allocator
add_real_library(vma INTERFACE)
real_target_init_file_sets(vma BASE_DIR ".")
add_subdirectory(vma)
