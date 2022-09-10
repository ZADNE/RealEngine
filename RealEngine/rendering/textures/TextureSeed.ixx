/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.textures.TextureSeed;
import std;


export namespace RE {

/**
 * @brief Manages conversion from filename to full path to a texture
*/
struct TextureSeed {

    std::string toFullPath() const {
        return s_folder + file + s_extension;
    }

    const std::string& file; /**< Filepath without extension */

private:

    static inline std::string s_folder = "textures/";
    static inline std::string s_extension = ".png";
};

}
