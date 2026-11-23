# author     Dubsky Tomas
include_guard()

function(_re_define_target_property_and_cache_var NAME)
    set(options "")
    set(one_value_keywords "TYPE")
    set(multi_value_keywords "HELP;VALUE")
    cmake_parse_arguments(PARSE_ARGN "ARG"
        "${options}" "${one_value_keywords}" "${multi_value_keywords}"
    )
    set(CACHE{${NAME}}
        TYPE ${ARG_TYPE}
        HELP "${ARG_HELP}"
        VALUE "${ARG_VALUE}"
    )
    define_property(TARGET
        PROPERTY ${NAME}
        BRIEF_DOCS "${ARG_HELP}"
        INITIALIZE_FROM_VARIABLE ${NAME}
    )
endfunction()

# File extension interpretation
_re_define_target_property_and_cache_var(
    RE_MODULE_INTERFACE_EXTENSIONS
    TYPE  STRING
    HELP  "Extensions interpreted as C++ interface or partition module units"
    VALUE ".cppm"
)
_re_define_target_property_and_cache_var(
    RE_HEADER_EXTENSIONS
    TYPE  STRING
    HELP  "Extensions interpreted as C++ header files"
    VALUE ".hpp"
)
_re_define_target_property_and_cache_var(
    RE_SOURCE_EXTENSIONS
    TYPE  STRING
    HELP  "Extensions interpreted as C++ source files"
    VALUE ".cpp"
)
_re_define_target_property_and_cache_var(
    RE_GLSL_HEADER_EXTENSIONS
    TYPE  STRING
    HELP  "Extensions interpreted as GLSL header files"
    VALUE ".glsl"
)
_re_define_target_property_and_cache_var(
    RE_GLSL_STAGE_EXTENSIONS
    TYPE  STRING
    HELP  "Extensions interpreted as GLSL stage files"
    VALUE ".vert;.tesc;.tese;.geom;.frag;.comp"
)

# Other custom properties with cache initialization
_re_define_target_property_and_cache_var(
    RE_SOURCES_AS_CXX_IMPL_UNITS
    TYPE  BOOL
    HELP  "Tells whether all source files are interpreted as module implementation units"
    VALUE OFF
)
_re_define_target_property_and_cache_var(
    RE_GLSL_STANDARD
    TYPE  STRING
    HELP  "The standard used to compile GLSL shaders"
    VALUE "460"
)
_re_define_target_property_and_cache_var(
    RE_SHADER_NAMESPACE
    TYPE  STRING
    HELP  "C++ wrapper code for shaders (headers and sources) will be put into this namespace."
    VALUE ""
)

set(CACHE RE_REAL_TARGET_DEFAULT_PROPERTY_OVERRIDES
    TYPE STRING
    HELP "Property name-value pairs that are set for all real targets."
    VALUE 
        CXX_STANDARD 23
        CXX_MODULE_STD ON
        INTERPROCEDURAL_OPTIMIZATION $<NOT:$<CONFIG:Debug>>
)
