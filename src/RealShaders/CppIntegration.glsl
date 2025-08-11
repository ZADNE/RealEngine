/*!
 *  @author     Dubsky Tomas
 */

#if !defined(RE_CPP_INTEGRATED)
#    define RE_CPP_INTEGRATED

#    if defined(VULKAN)
#        extension GL_EXT_scalar_block_layout : require

#        define RE_GLSL_ONLY(tokens) tokens
#        define RE_CPP_ONLY(tokens)

         // 'Introduce' keywords from C++
#        define constexpr const
#        define inline
#    else
#        define RE_GLSL_ONLY(tokens)
#        define RE_CPP_ONLY(tokens) tokens
#    endif
#endif

