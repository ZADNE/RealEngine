/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <filesystem>
#include <vector>

namespace re {

/**
 * @brief Reads a binary file into a buffer
 * @throws When file cannot be read.
 */
std::vector<unsigned char> readBinaryFile(const std::filesystem::path& path);

} // namespace re
