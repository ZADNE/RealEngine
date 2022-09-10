/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.batches.Circle;
import glm;
export import RealEngine.rendering.vertices.Vertex;


export namespace RE {

#pragma warning(push)
#pragma warning(disable: 26495)

/**
 * @brief Represents a 2D circle/disk.
*/
struct CirclePO {
    CirclePO() {}
    CirclePO(const glm::vec2& position, float radius, bool disC) : pos(position), rad(radius), disc(disC) {}

    glm::vec2 pos;
    float rad;
    bool disc;
};

/**
 * @brief Represents a colored 2D circle/disk.
*/
struct CirclePOCO : public CirclePO {
    CirclePOCO() {}
    //Solid color
    CirclePOCO(const CirclePO& circle, Color color) : CirclePO(circle), mid(color), edge(color) {}
    //Transition in colors
    CirclePOCO(const CirclePO& circle, Color middle, Color edgE) : CirclePO(circle), mid(middle), edge(edgE) {}

    Color mid;
    Color edge;
};

#pragma warning(pop)

}