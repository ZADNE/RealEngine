#author     Dubsky Tomas

function(RealShaders_GenerateCppFiles target scope path_rel)
    #Resolve Cpp namespace
    get_property(cpp_namespace_start TARGET ${target} PROPERTY SHADERS_CXX_NAMESPACE)
    if ("${cpp_namespace_start}" STREQUAL "")
        set(cpp_namespace_start "\n")
        set(cpp_namespace_end "\n")
    else()
        set(cpp_namespace_start "namespace ${cpp_namespace_start} {\n\n")
        set(cpp_namespace_end "\n}\n")
    endif()

    string(CONCAT cpp_preamble
        "//Automatically generated file by RealShaders\n")
    string(CONCAT hpp_preamble
        "//Automatically generated file by RealShaders\n"
        "#pragma once\n"
        "#include <RealEngine/rendering/pipelines/PipelineSources.hpp>\n\n"
        ${cpp_namespace_start})

    string(CONCAT folder_hpp
        ${hpp_preamble})
        
    set(stage_exts ".vert;.tesc;.tese;.geom;.frag;.comp")
    foreach(shader IN LISTS ARGN)
        get_filename_component(shader_ext ${shader} LAST_EXT)
        if (${shader_ext} IN_LIST stage_exts)
            #Compose declarations for the C++ constant
            string(REPLACE "." "_" shader_ ${shader})
            set(shader_declaration_line "extern RE::ShaderSource ${shader_}\;\n")
            string(APPEND folder_hpp "${shader_declaration_line}")
            string(CONCAT shader_hpp
                ${hpp_preamble}
                "${shader_declaration_line}"
                ${cpp_namespace_end})
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${shader_}.hpp" ${shader_hpp})
            #Compose definition for the constant
            set(shader_rel "${path_rel}/${shader}")
            string(CONCAT shader_cpp
                ${cpp_preamble}
                "#include <${path_rel}/${shader_}.hpp>\n\n"
                ${cpp_namespace_start}
                "#if 0\n"
                "//Navigation section - use #includes to jump to files (Visual Studio):\n"
                "//Source file:\n"
                "   #include <${path_rel}/${shader}>\n"
                "//Dependency file:\n"
                "   #include <${path_rel}/${shader}.d>\n"
                "//Disassembly file:\n"
                "   #include <${path_rel}/${shader}.spv_vk13.txt>\n"
                "#endif // 0 (navigation section)\n\n"
                "RE::ShaderSource ${shader_}{\n"
                "    .vk13 =\n"
                "    #include <${path_rel}/${shader}.spv_vk13>\n"
                "}\;\n"
                ${cpp_namespace_end})
            file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${shader_}.cpp" ${shader_cpp})
            target_sources(${target} ${scope} "${CMAKE_CURRENT_BINARY_DIR}/${shader_}.cpp")
        endif()
    endforeach()
    
    string(APPEND folder_hpp ${cpp_namespace_end})
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/AllShaders.hpp" ${folder_hpp})
endfunction()