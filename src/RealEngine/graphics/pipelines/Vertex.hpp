/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace re {

/**
 * @brief Is RGBA 8-bits-per-channel color
 */
using Color = glm::vec<4, uint8_t, glm::qualifier::defaultp>;

#pragma warning(push)
#pragma warning(disable : 26495)

/**
 * @brief Is POD vertex with position
 */
struct VertexPo {
    VertexPo() {}
    VertexPo(glm::vec2 pos)
        : position(pos) {}

    glm::vec2 position;
};

/**
 * @brief Is POD vertex with position and color
 */
struct VertexPoCo {
    VertexPoCo() {}
    VertexPoCo(glm::vec2 pos, Color col)
        : position(pos)
        , color(col) {}

    glm::vec2 position;
    Color     color;
};

/**
 * @brief Is POD vertex with position, color and UVs
 */
struct VertexPoCoUv {
    VertexPoCoUv() {}
    VertexPoCoUv(glm::vec2 pos, Color col, glm::vec2 uv)
        : position(pos)
        , color(col)
        , uv(uv) {}

    glm::vec2 position;
    Color     color;
    glm::vec2 uv;
};

#pragma warning(pop)

} // namespace re
