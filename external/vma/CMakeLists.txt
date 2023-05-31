add_library(vma INTERFACE)

target_sources(vma INTERFACE
    vk_mem_alloc_enums.hpp
    vk_mem_alloc_funcs.hpp
    vk_mem_alloc_handles.hpp
    vk_mem_alloc_structs.hpp
    vk_mem_alloc.hpp
)
