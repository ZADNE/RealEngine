/**
 *  @author    Dubsky Tomas
 */
#include <fstream>

#include <RealEngine/resources/FileIO.hpp>

namespace re {

std::vector<unsigned char> readBinaryFile(const std::filesystem::path& path) {
    std::vector<unsigned char> out;
    readBinaryFile(path, out);
    return out;
}

void readBinaryFile(const std::filesystem::path& path, std::vector<unsigned char>& out) {
    size_t size = static_cast<size_t>(std::filesystem::file_size(path));
    out.resize(size);
    std::ifstream file{path, std::ios::binary};
    file.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
}

std::string readTextFile(const std::filesystem::path& fullPath) {
    size_t binarySize = static_cast<size_t>(std::filesystem::file_size(fullPath));
    std::string content;
    content.reserve(binarySize); // Size may be smaller due to \r\n -> \n conversion
    std::ifstream file(fullPath);
    using StreamIter = std::istreambuf_iterator<char>;
    content.assign(StreamIter{file}, StreamIter{});
    return content;
}

} // namespace re
