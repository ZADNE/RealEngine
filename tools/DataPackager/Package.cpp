/*!
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <fstream>

#include <bit7z/bitfilecompressor.hpp>

#include <RealEngine/resources/PackageLoader.hpp>

#include <DataPackager/Package.hpp>

namespace fs = std::filesystem;

namespace re::dp {

constexpr const char* k_indexFile =
    "/*!\n"
    " * Automatically generated file by RealEngine's DataPackager\n"
    " */\n"
    "#pragma once\n"
    "#include <vector>\n"
    "\n"
    "#include <RealEngine/resources/ResourceManager.hpp>\n"
    "\n"
    "namespace re {{\n"
    "\n"
    "consteval ResourceID resourceID(const char* path) {{\n"
    "    std::vector<std::string> k_index{{{}}};\n"
    "    for (uint32_t i = 0; i < k_index.size(); ++i) {{\n"
    "        if (k_index[i] == path) {{\n"
    "            return ResourceID{{i, \"\"}};\n"
    "        }}\n"
    "    }}\n"
    "    return ResourceID{{0xffffffff, \"\"}};\n"
    "}}\n"
    "\n"
    "consteval ResourceID textureID(const char* filenameStem) {{\n"
    "    auto path = std::string{{\"textures/\"}} + filenameStem + \".png\";\n"
    "    return resourceID(path.c_str());\n"
    "}}\n"
    "\n"
    "}} // namespace re\n";

std::string toForwardSlash(const std::string& in) {
    std::string rval;
    rval.reserve(in.size());
    for (char c : in) { rval.push_back(c == '\\' ? '/' : c); }
    return rval;
}

void composePackage(
    std::span<const std::string> inputDirs, const std::string& outputDir,
    const std::string& indexFilepath
) {
    // Prepare 7z
    bit7z::Bit7zLibrary lib{};
    bit7z::BitFileCompressor compressor{lib, bit7z::BitFormat::SevenZip};
    compressor.setPassword(PackageLoader::k_packageKey, true);
    bit7z::BitOutputArchive outputArchive{compressor};
    std::vector<std::string> index;

    // Collect files the files
    for (fs::path inputDir : inputDirs) {
        fs::path parentDir = inputDir.parent_path();
        for (const auto& entry : fs::recursive_directory_iterator{inputDir}) {
            if (entry.is_regular_file()) {
                // Replace path with index to obfuscate (path will not be used by runtime)
                std::string archivePath = std::to_string(index.size());
                // Add the file to archive
                outputArchive.addFile(entry.path().string(), entry.path().string());
                // Index path (with forward slashes as separator for consistency)
                index.push_back(
                    toForwardSlash(fs::relative(entry.path(), parentDir).string())
                );
            }
        }
    }

    // Delete previous package and create the new package
    auto outputFilepath = fs::path{outputDir} / PackageLoader::k_packageName;
    fs::remove(outputFilepath);
    outputArchive.compressTo(outputFilepath.string());

    // Compose C++ index
    std::string initializer;
    for (int i = 0; i < static_cast<int>(index.size()) - 1; ++i) {
        initializer += std::format("\"{}\", ", index[i]);
    }
    initializer += std::format("\"{}\"", index[index.size() - 1]);
    std::string indexContents = std::format(k_indexFile, initializer);

    // Write C++ index
    std::ofstream indexFile{indexFilepath};
    indexFile.write(indexContents.data(), indexContents.size());
}

} // namespace re::dp
