# Disable linting for external targets
set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(ImGui)
add_subdirectory(lodepng)
add_subdirectory(vma)
