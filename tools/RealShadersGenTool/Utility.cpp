/*!
 *  @author    Dubsky Tomas
 */
#include <RealShadersGenTool/Utility.hpp>

namespace rsg {

std::string readWholeFile(const std::filesystem::path& fullPath) {
    size_t size = static_cast<size_t>(std::filesystem::file_size(fullPath));
    std::string content(size, '\0');
    std::ifstream file(fullPath, std::ios::binary);
    file.read(content.data(), size);
    return content;
}

} // namespace rsg
