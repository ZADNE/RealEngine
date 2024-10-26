/*!
 *  @author    Dubsky Tomas
 */
#include <fstream>
#include <iterator>

#include <RealShadersGenTool/Utility.hpp>

namespace rsg {

using StreamIter = std::istreambuf_iterator<char>;

std::string readWholeFile(const std::filesystem::path& fullPath) {
    size_t binarySize = static_cast<size_t>(std::filesystem::file_size(fullPath));
    std::string content;
    content.reserve(binarySize); // Size may be smaller due to \r\n -> \n conversion
    std::ifstream file(fullPath);
    content.assign(StreamIter{file}, StreamIter{});
    return content;
}

} // namespace rsg
