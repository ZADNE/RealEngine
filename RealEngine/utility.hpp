#pragma once
#include <algorithm>
#include <string>

#include <GL/glew.h>
#include <glm/vec2.hpp>

namespace rmath {

	template <typename T> int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper) {
		return (std::max)(lower, (std::min)(n, upper));
	}

	float distance(const glm::ivec2& p1, const glm::ivec2& p2);
	bool isInsideCircle(const glm::ivec2& point, const glm::ivec2& center, float radius);

	const float PI_f = 3.14159265358979323846264338327950288f;
	const double PI = 3.14159265358979323846264338327950288;
	const long double PI_L = 3.14159265358979323846264338327950288L;
}

namespace RE {

	std::string GLSLTypeToString(GLenum type);

}

//WinAPI uses the same macro
#ifdef RGB
#undef RGB
#endif // RGB



#define STR(x) #x

#define XY(vec) (vec).x, (vec).y
#define XYZ(vec) (vec).x, (vec).y, (vec).z
#define XYZW(vec) (vec).x, (vec).y, (vec).z, (vec).w

#define RG(vec) (vec).r, (vec).g
#define RGB(vec) (vec).r, (vec).g, (vec).b
#define RGBA(vec) (vec).r, (vec).g, (vec).b, (vec).a

#define ST(vec) (vec).s, (vec).t
#define STP(vec) (vec).s, (vec).t, (vec).p
#define STPQ(vec) (vec).s, (vec).t, (vec).p, (vec).q

//SMART ENUMS @ David Saltares

/*
Example usage

#define KB_BINDING_LIST(m)   \
	m(VIEW, MOVE_LEFT)  \
	m(VIEW, ZOOM_OUT)  \
	m(BUILDMENU, OPEN)  \
	m(BUILDMENU, CHOOSE)  \
	m(DEBUG, ENDGAME)  \
	m(DEBUG, WORLDDRAW)

	VIEW, BUILDMENU, DEBUG = category
	MOVE_LEFT, ZOOM_OUT, ... = identifier

	category + identifier = name

	namespace {
		SMARTENUM_DEFINE_ENUM(BL, KB_BINDING_LIST)
		SMARTENUM_DEFINE_NAMES(BL, KB_BINDING_LIST)
		SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(BL, KB_BINDING_LIST)
		SMARTENUM_DEFINE_CATEGORIES(BL, KB_BINDING_LIST)
	}


*/


#define SMARTENUM_VALUE(typeName, value) e##typeName##_##value,
#define SMARTENUM_DEFINE_ENUM(typeName, values) enum typeName { values(SMARTENUM_VALUE) e##typeName##_Error, e##typeName##_Count, };

#define SMARTENUM_STRING(typeName, value) STR(e##typeName##_##value),
#define SMARTENUM_DEFINE_NAMES(typeName, values) const char* typeName##Array [] = { values(SMARTENUM_STRING) "ERROR_NAME", };
#define enumToString(typeName, value) ((value < e##typeName##_Count && value >= 0) ? typeName##Array[##value] : "ERROR_NAME")

#define SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(typeName, name)       \
	typeName get##typeName##FromString(const std::string& str)     \
	{                               \
		for (int i = 0; i < e##typeName##_Count; ++i)    \
			if (!strcmp(##typeName##Array[i], str.c_str()))     \
				return (##typeName##)i;         \
		return e##typeName##_Error;             \
	}
#define stringToEnum(typeName, name)  get##typeName##FromString(##name)

#define SMARTENUM_CATEGORY(typeName, value) STR(typeName),
#define SMARTENUM_DEFINE_CATEGORIES(typeName, values) const char* typeName##Cat [] = { values(SMARTENUM_CATEGORY) "ERROR_CAT", };
#define enumToCat(typeName, value) ((value < e##typeName##_Count && value >= 0) ? typeName##Cat[##value] : "ERROR_CAT")

#define SMARTENUM_IDENTIFIER(typeName, value) STR(value),
#define SMARTENUM_DEFINE_IDENTIFIERS(typeName, values) const char* typeName##Iden [] = { values(SMARTENUM_IDENTIFIER) "ERROR_IDEN", };
#define enumToIden(typeName, value) ((value < e##typeName##_Count && value >= 0) ? typeName##Iden[##value] : "ERROR_IDEN")

#define SMARTENUM_INIT_ALL(typeName, value) \
	SMARTENUM_DEFINE_ENUM(typeName, value) \
	SMARTENUM_DEFINE_NAMES(typeName, value) \
	SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(typeName, value) \
	SMARTENUM_DEFINE_CATEGORIES(typeName, value) \
	SMARTENUM_DEFINE_IDENTIFIERS(typeName, value)