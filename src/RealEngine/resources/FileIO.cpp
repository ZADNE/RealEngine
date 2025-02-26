/**
 *  @author    Dubsky Tomas
 */
#include <fstream>

#include <RealEngine/resources/FileIO.hpp>

namespace re {

std::vector<unsigned char> readBinaryFile(const std::filesystem::path& path) {
    size_t size = static_cast<size_t>(std::filesystem::file_size(path));
    std::vector<unsigned char> content(size, '\0');
    std::ifstream file{path, std::ios::binary};
    file.read(
        reinterpret_cast<char*>(content.data()), static_cast<std::streamsize>(size)
    );
    return content;
}

} // namespace re
