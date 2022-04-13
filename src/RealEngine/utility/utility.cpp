﻿#include <RealEngine/utility/utility.hpp>

#include <cmath>

namespace rmath {

float distance(const glm::ivec2& p1, const glm::ivec2& p2) {
	int xx = p1.x - p2.x;
	int yy = p1.y - p2.y;
	return std::sqrt(static_cast<float>(xx * xx + yy * yy));
}

bool isInsideCircle(const glm::ivec2& point, const glm::ivec2& center, float radius) {
	if (distance(point, center) <= radius) {
		return true;
	}
	return false;
}

}

std::string_view RE::GLSLTypeToString(GLenum type) {
	switch (type) {
		//SCALARS
	case GL_FLOAT:
		return "float";
	case GL_DOUBLE:
		return "double";
	case GL_BOOL:
		return "bool";
	case GL_INT:
		return "int";
	case GL_UNSIGNED_INT:
		return "uint";
		//VECTORS
	case GL_FLOAT_VEC2:
		return "vec2";
	case GL_FLOAT_VEC3:
		return "vec3";
	case GL_FLOAT_VEC4:
		return "vec4";
	case GL_DOUBLE_VEC2:
		return "dvec2";
	case GL_DOUBLE_VEC3:
		return "dvec3";
	case GL_DOUBLE_VEC4:
		return "dvec4";
	case GL_BOOL_VEC2:
		return "bvec2";
	case GL_BOOL_VEC3:
		return "bvec3";
	case GL_BOOL_VEC4:
		return "bvec4";
	case GL_INT_VEC2:
		return "ivec2";
	case GL_INT_VEC3:
		return "ivec3";
	case GL_INT_VEC4:
		return "ivec4";
	case GL_UNSIGNED_INT_VEC2:
		return "uvec2";
	case GL_UNSIGNED_INT_VEC3:
		return "uvec3";
	case GL_UNSIGNED_INT_VEC4:
		return "uvec4";
		//SQUARE MATRICES
	case GL_FLOAT_MAT2:
		return "mat2";
	case GL_FLOAT_MAT3:
		return "mat3";
	case GL_FLOAT_MAT4:
		return "mat4";
	case GL_DOUBLE_MAT2:
		return "dmat2";
	case GL_DOUBLE_MAT3:
		return "dmat3";
	case GL_DOUBLE_MAT4:
		return "dmat4";
		//RECTANGLE MATRICES
	case GL_FLOAT_MAT2x3:
		return "mat2x3";
	case GL_FLOAT_MAT2x4:
		return "mat2x4";
	case GL_FLOAT_MAT3x2:
		return "mat3x2";
	case GL_FLOAT_MAT3x4:
		return "mat3x4";
	case GL_FLOAT_MAT4x2:
		return "mat4x2";
	case GL_FLOAT_MAT4x3:
		return "mat4x3";
	case GL_DOUBLE_MAT2x3:
		return "dmat2x3";
	case GL_DOUBLE_MAT2x4:
		return "dmat2x4";
	case GL_DOUBLE_MAT3x2:
		return "dmat3x2";
	case GL_DOUBLE_MAT3x4:
		return "dmat3x4";
	case GL_DOUBLE_MAT4x2:
		return "dmat4x2";
	case GL_DOUBLE_MAT4x3:
		return "dmat4x3";
		//1D SAMPLERS
	case GL_SAMPLER_1D:
		return "sampler1D";
	case GL_SAMPLER_1D_SHADOW:
		return "sampler1DShadow";
	case GL_SAMPLER_1D_ARRAY:
		return "sampler1DArray";
	case GL_SAMPLER_1D_ARRAY_SHADOW:
		return "sampler1DArrayShadow";
	case GL_INT_SAMPLER_1D:
		return "isampler1D";
	case GL_INT_SAMPLER_1D_ARRAY:
		return "isampler1DArray";
	case GL_UNSIGNED_INT_SAMPLER_1D:
		return "usampler1D";
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		return "usampler2DArray";
		//2D SAMPLERS
	case GL_SAMPLER_2D:
		return "sampler2D";
	case GL_SAMPLER_2D_SHADOW:
		return "sampler2DShadow";
	case GL_SAMPLER_2D_ARRAY:
		return "sampler2DArray";
	case GL_SAMPLER_2D_ARRAY_SHADOW:
		return "sampler2DArrayShadow";
	case GL_SAMPLER_2D_MULTISAMPLE:
		return "sampler2DMS";
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		return "sampler2DMSArray";
	case GL_SAMPLER_2D_RECT:
		return "sampler2DRect";
	case GL_SAMPLER_2D_RECT_SHADOW:
		return "sampler2DRectShadow";
	case GL_INT_SAMPLER_2D:
		return "isampler2D";
	case GL_INT_SAMPLER_2D_ARRAY:
		return "isampler2DArray";
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
		return "isampler2DMS";
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		return "isampler2DMSArray";
	case GL_INT_SAMPLER_2D_RECT:
		return "isampler2DRect";
	case GL_UNSIGNED_INT_SAMPLER_2D:
		return "usampler2D";
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		return "usampler2DArray";
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		return "usampler2DMS";
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		return "usampler2DMSArray";
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		return "usampler2DRect";
		//3D SAMPLERS
	case GL_SAMPLER_3D:
		return "sampler3D";
	case GL_INT_SAMPLER_3D:
		return "isampler3D";
	case GL_UNSIGNED_INT_SAMPLER_3D:
		return "usampler3D";
		//CUBE SAMPLERS
	case GL_SAMPLER_CUBE:
		return "samplerCube";
	case GL_SAMPLER_CUBE_SHADOW:
		return "samplerCubeShadow";
	case GL_INT_SAMPLER_CUBE:
		return "isamplerCube";
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
		return "usamplerCube";
		//BUFFER SAMPLERS
	case GL_SAMPLER_BUFFER:
		return "samplerBuffer";
	case GL_INT_SAMPLER_BUFFER:
		return "isamplerBuffer";
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		return "usamplerBuffer";
		//1D IMAGES
	case GL_IMAGE_1D:
		return "image1D";
	case GL_IMAGE_1D_ARRAY:
		return "image1DArray";
	case GL_INT_IMAGE_1D:
		return "iimage1D";
	case GL_INT_IMAGE_1D_ARRAY:
		return "iimage1DArray";
	case GL_UNSIGNED_INT_IMAGE_1D:
		return "uimage1D";
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
		return "uimage2DArray";
		//2D IMAGES
	case GL_IMAGE_2D:
		return "image2D";
	case GL_IMAGE_2D_ARRAY:
		return "image2DArray";
	case GL_IMAGE_2D_MULTISAMPLE:
		return "image2DMS";
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
		return "image2DMSArray";
	case GL_IMAGE_2D_RECT:
		return "image2DRect";
	case GL_INT_IMAGE_2D:
		return "iimage2D";
	case GL_INT_IMAGE_2D_ARRAY:
		return "iimage2DArray";
	case GL_INT_IMAGE_2D_MULTISAMPLE:
		return "iimage2DMS";
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
		return "iimage2DMSArray";
	case GL_INT_IMAGE_2D_RECT:
		return "iimage2DRect";
	case GL_UNSIGNED_INT_IMAGE_2D:
		return "uimage2D";
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
		return "uimage2DArray";
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
		return "uimage2DMS";
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
		return "uimage2DMSArray";
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:
		return "uimage2DRect";
		//3D IMAGES
	case GL_IMAGE_3D:
		return "image3D";
	case GL_INT_IMAGE_3D:
		return "iimage3D";
	case GL_UNSIGNED_INT_IMAGE_3D:
		return "uimage3D";
		//CUBE IMAGES
	case GL_IMAGE_CUBE:
		return "imageCube";
	case GL_INT_IMAGE_CUBE:
		return "iimageCube";
	case GL_UNSIGNED_INT_IMAGE_CUBE:
		return "uimageCube";
		//BUFFER IMAGES
	case GL_IMAGE_BUFFER:
		return "imageBuffer";
	case GL_INT_IMAGE_BUFFER:
		return "iimageBuffer";
	case GL_UNSIGNED_INT_IMAGE_BUFFER:
		return "uimageBuffer";
		//ATOMIC COUNTER
	case GL_UNSIGNED_INT_ATOMIC_COUNTER:
		return "atomic_uint";
	default:
		return "ERROR! Unknown OpenGL type for parser - does not mean it is unknown to OpenGL!";
	}
}