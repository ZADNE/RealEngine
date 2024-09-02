# Disable linting for external targets
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(ImGui)
add_subdirectory(lodepng)
add_subdirectory(vma)
