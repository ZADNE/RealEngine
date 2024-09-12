add_library(vma INTERFACE)

target_sources(vma
    INTERFACE FILE_SET HEADERS BASE_DIRS ".." FILES
        vk_mem_alloc.h
        vk_mem_alloc.hpp
        vk_mem_alloc_enums.hpp
        vk_mem_alloc_funcs.hpp
        vk_mem_alloc_handles.hpp
        vk_mem_alloc_structs.hpp
)
