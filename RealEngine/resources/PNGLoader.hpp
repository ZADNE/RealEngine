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
     * @brief   Loads texels and parameters from PNG file
     * @return  0 on success, other on failure
     * @throws  Throws when the file cannot be opened/decoded
     * @details Failing to load parameters is not considered a failure
    */
    static PNGData load(const std::string& filePathPNG);

    /**
     * @brief   Saves texels and parameters to PNG file
     * @return  0 on success, other on failure
     * @details Overwrites existing file without notice
     * @throws  Throws when the file cannot be saved/encoded
    */
    static void save(const std::string& filePathPNG, const PNGData& data);

    /**
     * @brief   Replaces parameters inside PNG file
     * @return  0 on success, other on failure
     * @throws  Throws when the file cannot be opened/decoded
    */
    static void replaceParameters(const std::string& filePathPNG, const TextureShape& shape);
};

}