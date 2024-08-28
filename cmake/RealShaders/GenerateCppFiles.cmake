# author    Dubsky Tomas

function(RealShaders_GenerateCppFiles target scope path_rel)
    #Resolve Cpp namespace
    get_property(cpp_namespace TARGET ${target} PROPERTY REALSHADERS_CXX_NAMESPACE)
    if ("${cpp_namespace}" STREQUAL "")
        set(cpp_namespace_start "\n")
        set(cpp_namespace_end "\n")
    else()
        set(cpp_namespace_start "namespace ${cpp_namespace} {\n\n")
        set(cpp_namespace_end "\n} // namespace ${cpp_namespace}\n")
    endif()

    # Compose preamble
    string(TIMESTAMP timestamp)
    string(CONCAT cpp_preamble
        "// Automatically generated file by RealShaders at ${timestamp}\n")
    string(CONCAT hpp_preamble
        "// Automatically generated file by RealShaders at ${timestamp}\n"
        "#pragma once\n"
        "#include <RealEngine/graphics/pipelines/PipelineSources.hpp>\n\n"
        ${cpp_namespace_start})

    string(CONCAT folder_hpp
        ${hpp_preamble})
        
    set(stage_exts ".vert;.tesc;.tese;.geom;.frag;.comp")
    foreach(shader IN LISTS ARGN)
        get_filename_component(shader_ext ${shader} LAST_EXT)
        if (${shader_ext} IN_LIST stage_exts)
            string(REPLACE "." "_" shader_ ${shader})
            set(shader_declaration_line "extern const re::ShaderSource ${shader_}\;\n")
            string(APPEND folder_hpp "${shader_declaration_line}")

            # Create C++ header for the shader
            string(CONCAT shader_hpp
                ${hpp_preamble}
                "${shader_declaration_line}"
                ${cpp_namespace_end})
            file(GENERATE
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${shader_}.hpp"
                CONTENT ${shader_hpp})
            
            # Create C++ source for the shader
            string(CONCAT shader_cpp
                ${cpp_preamble}
                "#include <${path_rel}/${shader_}.hpp>\n\n"
                ${cpp_namespace_start}
                "#if 0\n"
                "// Navigation section - use #includes to jump to files (Visual Studio):\n"
                "// Source file:\n"
                "   #include <${path_rel}/${shader}>\n"
                "// Dependency file:\n"
                "   #include <${path_rel}/${shader}.d>\n"
                "// Disassembly file:\n"
                "   #include <${path_rel}/${shader}.spv_vk13.txt>\n"
                "#endif // 0 (navigation section)\n\n"
                "const re::ShaderSource ${shader_}{\n"
                "    .vk13 =\n"
                "    #include <${path_rel}/${shader}.spv_vk13>\n"
                "}\;\n"
                ${cpp_namespace_end})
            set(cpp_abs "${CMAKE_CURRENT_BINARY_DIR}/${shader_}.cpp")
            file(GENERATE
                OUTPUT ${cpp_abs}
                CONTENT ${shader_cpp})
            target_sources(${target} ${scope} ${cpp_abs})
        endif()
    endforeach()

    string(APPEND folder_hpp ${cpp_namespace_end})
    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AllShaders.hpp"
        CONTENT ${folder_hpp})
endfunction()