/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

#include <RealEngine/rendering/textures/TextureShape.hpp>

namespace RE {

/**
 * @brief Loads/saves texels and parameters from/to PNG file
*/
class PNGLoader {
public:

    /**
     * @brief Is POD structure of data that can be stored in a PNG file
    */
    struct PNGData {
        std::vector<unsigned char> texels;
        glm::uvec2 dims;
        TextureShape shape;
    };

    /**
     * @brief Loads texels and parameters from PNG file
     * @return 0 on success, other on failure
     *
     * Failing to load parameters is not considered a failure.
    */
    static PNGData load(const std::string& filePathPNG);

    /**
     * @brief Saves texels and parameters to PNG file
     * @return 0 on success, other on failure
     *
     * Overwrites existing file without notice.
    */
    static unsigned int save(const std::string& filePathPNG, const PNGData& data);
};

}