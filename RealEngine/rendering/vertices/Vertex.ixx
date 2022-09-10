/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.vertices.Vertex;
import std;
import glm;

export namespace RE {

/**
 * @brief Is RGBA 8-bits-per-channel color
*/
using Color = glm::vec<4, uint8_t, glm::qualifier::defaultp>;

constexpr unsigned int ATTR_POSITION = 0u;
constexpr unsigned int ATTR_COLOR = 1u;
constexpr unsigned int ATTR_UV = 2u;

template<typename T>
consteval T PRIMITIVE_RESTART_INDEX() {
    static_assert(std::is_unsigned<T>::value, "primitive restart index in defined only for unsigned types");
    return std::numeric_limits<T>::max();
};

#pragma warning(push)
#pragma warning(disable: 26495)

/**
* @brief Is POD vertex with position
*/
struct VertexPO {

    VertexPO() {}
    VertexPO(const glm::vec2& pos) : position(pos) {}

    glm::vec2 position;
};

/**
* @brief Is POD vertex with position and color
*/
struct VertexPOCO {

    VertexPOCO() {}
    VertexPOCO(const glm::vec2& pos, Color col) : position(pos), color(col) {}

    glm::vec2 position;
    Color color;
};

/**
* @brief Is POD vertex with position, color and UVs
*/
struct VertexPOCOUV {

    VertexPOCOUV() {}
    VertexPOCOUV(const glm::vec2& pos, Color col, const glm::vec2& uv) : position(pos), color(col), uv(uv) {}

    glm::vec2 position;
    Color color;
    glm::vec2 uv;
};

#pragma warning(pop)

}
