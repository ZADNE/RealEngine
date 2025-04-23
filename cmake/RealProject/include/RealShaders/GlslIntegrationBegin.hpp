/*!
 *  @author    Dubsky Tomas
 */

// Suppress GLSL keywords irrelevant for C++
#define RE_LAYOUT_1(_1)
#define RE_LAYOUT_2(_1, _2)
#define RE_LAYOUT_3(_1, _2, _3)
#define RE_LAYOUT_4(_1, _2, _3, _4)
#define RE_LAYOUT_5(_1, _2, _3, _4, _5)
#define RE_LAYOUT_6(_1, _2, _3, _4, _5, _6)
#define RE_LAYOUT_7(_1, _2, _3, _4, _5, _6, _7)
#define RE_LAYOUT_8(_1, _2, _3, _4, _5, _6, _7, _8)
#define RE_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define layout(...)                                                      \
    RE_GET_MACRO(                                                        \
        __VA_ARGS__, RE_LAYOUT_8, RE_LAYOUT_7, RE_LAYOUT_6, RE_LAYOUT_5, \
        RE_LAYOUT_4, RE_LAYOUT_3, RE_LAYOUT_2, RE_LAYOUT_1               \
    )                                                                    \
    (__VA_ARGS__)
#define coherent
#define restrict
#define readonly
#define writeonly
#define buffer struct
#define uniform struct
#define in
#define inout inout_parameter_qualifier_cannot_be_exposed_to_cpp
#define out out_parameter_qualifier_cannot_be_exposed_to_cpp

using namespace glm;

// Allow variable-length arrays
#pragma warning(push)
#pragma warning(disable : 4200)

#include <RealShaders/DefineSwizzlingMacros.hpp>
