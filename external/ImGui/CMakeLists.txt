﻿add_library(ImGui STATIC)
set_property(TARGET ImGui PROPERTY COMPILE_WARNING_AS_ERROR OFF)

target_link_libraries(ImGui PRIVATE
    Vulkan::Vulkan
    SDL2::Main
    glm
)

target_sources(ImGui PRIVATE
    imconfig.h                  
    imgui.h                     imgui.cpp
                                imgui_draw.cpp
    imgui_impl_sdl2.h           imgui_impl_sdl2.cpp
    imgui_impl_vulkan.h         imgui_impl_vulkan.cpp
    imgui_internal.h            
    imgui_stdlib.h              imgui_stdlib.cpp
                                imgui_tables.cpp
                                imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)
