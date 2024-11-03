/*!
 *  @author    Dubsky Tomas
 */
#include <fstream>
#include <iterator>

#include <RealShadersGenTool/Utility.hpp>

namespace rsg {

using StreamIter = std::istreambuf_iterator<char>;

std::string readWholeFile(const std::filesystem::path& fullPath) {
    namespace fs = std::filesystem;
    try {
        size_t binarySize = static_cast<size_t>(fs::file_size(fullPath));
        std::string content;
        // Size may be smaller due to \r\n -> \n conversion
        content.reserve(binarySize);
        std::ifstream file(fullPath);
        content.assign(StreamIter{file}, StreamIter{});
        return content;
    } catch (fs::filesystem_error& e) { fatalError("{}", e.what()); }
}

} // namespace rsg
