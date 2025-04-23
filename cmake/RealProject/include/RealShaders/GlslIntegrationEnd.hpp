/*!
 *  @author    Dubsky Tomas
 */

#undef RE_LAYOUT_1
#undef RE_LAYOUT_2
#undef RE_LAYOUT_3
#undef RE_LAYOUT_4
#undef RE_LAYOUT_5
#undef RE_LAYOUT_6
#undef RE_LAYOUT_7
#undef RE_LAYOUT_8
#undef RE_GET_MACRO
#undef layout
#undef coherent
#undef readonly
#undef writeonly
#undef restrict
#undef buffer
#undef uniform
#undef in
#undef inout
#undef out

// End scope for variable-length arrays
#pragma warning(pop)

#ifdef RE_CPP_INTEGRATED
#    undef RE_GLSL_ONLY
#    undef RE_CPP_ONLY
#    undef RE_CPP_INTEGRATED
#endif

#include <RealShaders/UndefSwizzlingMacros.hpp>
