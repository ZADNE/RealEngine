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
void readBinaryFile(const std::filesystem::path& path, std::vector<unsigned char>& out);

/**
 * @brief Reads a text file into a string
 * @throws When file cannot be read.
 */
std::string readTextFile(const std::filesystem::path& fullPath);

} // namespace re
