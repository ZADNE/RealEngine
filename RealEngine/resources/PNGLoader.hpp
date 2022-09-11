/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

#include <RealEngine/rendering/textures/TextureParameters.hpp>

namespace RE {

/**
 * @brief Loads/saves texture pixels and parameters from/to PNG file
*/
class PNGLoader {
public:

    /**
     * @brief Is POD structure of data that can be stored in a PNG file
    */
    struct PNGData {
        std::vector<unsigned char> pixels;
        glm::uvec2 dims;
        TextureParameters params;
    };

    /**
     * @brief Loads texture pixels and parameters from PNG file
     * @return 0 on success, other on failure
     *
     * Failing to load parameters is not considered a failure.
    */
    static unsigned int load(const std::string& filePathPNG, PNGData& data);

    /**
     * @brief Saves texture pixels and parameters to PNG file
     * @return 0 on success, other on failure
     *
     * Overwrites existing file without notice.
    */
    static unsigned int save(const std::string& filePathPNG, const PNGData& data);
};

}