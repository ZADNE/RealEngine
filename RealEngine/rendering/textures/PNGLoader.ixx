/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.textures.PNGLoader;
import std;
import glm;
import <lodepng/lodepng.hpp>;
export import RealEngine.rendering.textures.TextureParameters;


export namespace RE {

/**
 * @brief Loads texture pixels and parameters from PNG file
*/
class PNGLoader {
public:

    struct LoadedData {
        std::vector<char> pixels;
        glm::uvec2 dims;
        TextureChannels channels;
        TextureParameters params;
    };

    /**
     * @brief Loads texture pixels and parameters from PNG file
     * @return 0 on success, other on failure
     * 
     * Failing to load parameters is not considered a failure. 
    */
    static unsigned int load(const std::string& filePathPNG, LoadedData& tex);
};

}
