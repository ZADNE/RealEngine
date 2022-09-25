target_sources(${PROJECT_NAME} PRIVATE
    imconfig.h                  
    imgui.h                     imgui.cpp
                                imgui_draw.cpp
    imgui_impl_opengl3.h        imgui_impl_opengl3.cpp
    imgui_impl_opengl3_loader.h 
    imgui_impl_sdl.h            imgui_impl_sdl.cpp
    imgui_impl_vulkan.h         imgui_impl_vulkan.cpp
    imgui_internal.h            
    imgui_stdlib.h              imgui_stdlib.cpp
                                imgui_tables.cpp
                                imgui_widgets.cpp
    imstb_rectpack.h            
    imstb_textedit.h            
    imstb_truetype.h            
)
