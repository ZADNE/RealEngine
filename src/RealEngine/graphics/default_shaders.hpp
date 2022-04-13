#pragma once
#include <string_view>

namespace RE {

static std::string_view sprite_vert = {
	"layout(location = 0) in vec2 position;"
	"layout(location = 1) in vec4 colour;"
	"layout(location = 2) in vec2 UV;"

	"out vec4 fragColour;"
	"out vec2 fragUV;"

	"layout(std140, binding = 0) uniform GlobalMatrices {"
		"mat4 projectionMatrix;"
	"};"

	"void main() {"
		"gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);"
		"fragColour = colour;"
		"fragUV = vec2(UV.x, 1.0 - UV.y);"
	"}"
};

static int LOC_BASE_TEXTURE = 0;

static std::string_view sprite_frag = {
	"in vec4 fragColour;"
	"in vec2 fragUV;"

	"out vec4 colour;"

	"layout(location = 1) uniform sampler2D baseTexture;"

	"void main() {"
		"colour = fragColour * texture(baseTexture, fragUV);"
	"}"
};

static std::string_view geometry_vetr = {
	"layout(location = 0) in vec2 position;"
	"layout(location = 1) in vec4 colour;"

	"out vec4 fragColour;"

	"layout(std140, binding = 0) uniform GlobalMatrices {"
		"mat4 projectionMatrix;"
	"};"

	"void main() {"
		"gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);"
		"fragColour = colour;"
	"}"
};

static std::string_view geometry_frag = {
	"in vec4 fragColour;"

	"out vec4 colour;"

	"void main() {"
		"colour = fragColour;"
	"}"
};

}