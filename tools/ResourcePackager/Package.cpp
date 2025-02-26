/**
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <fstream>

#include <bit7z/bitfilecompressor.hpp>

#include <RealEngine/resources/PackageConstants.hpp>

#include <ResourcePackager/Package.hpp>

namespace fs = std::filesystem;

namespace re::rp {

constexpr const char* k_indexFileFormatString =
    "/**\n"
    " *  @file Automatically generated file by RealEngine's ResourcePackager\n"
    " */\n"
    "#pragma once\n"
    "#include <RealEngine/resources/ResourceID.hpp>\n"
    "#include <RealEngine/utility/CompTimeString.hpp>\n"
    "\n"
    "namespace re {{\n"
    "\n"
    "template<CompTimeString k_lit>\n"
    "consteval ResourceID resourceID() {{\n"
    "    if constexpr (k_buildType == BuildType::Debug) {{\n"
    "        // Allow non-indexed data in debug builds.\n"
    "        // The debug-only path will be used to load the data.\n"
    "        return ResourceID{{0xffffffff, k_lit.data}};\n"
    "    }} else {{\n"
    "        // All data must be indexed in release builds.\n"
    "        // This is because paths are not preserved at all.\n"
    "        throw \"Trying to use a non-indexed resource.\";\n"
    "    }}\n"
    "}};\n"
    "\n"
    "{}"
    "}} // namespace re\n";

constexpr const char* k_textureIDFormatString =
    "template<>\n"
    "consteval ResourceID resourceID<\"{0}\">() {{\n"
    "    return ResourceID{{{1}, \"{0}\"}};\n"
    "}};\n"
    "\n";

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
    compressor.setPassword(k_packageKey, true);
    bit7z::BitOutputArchive outputArchive{compressor};
    std::vector<std::string> index;

    // Collect files the files
    for (fs::path inputDir : inputDirs) {
        fs::path parentDir = inputDir.parent_path();
        for (const auto& entry : fs::recursive_directory_iterator{inputDir}) {
            if (entry.is_regular_file()) {
                // Replace path with index to obfuscate (path will not be used
                // by runtime).
                // It also ensures that the ordering within the zip stays the same.
                std::string archivePath = std::format("{:0>6}", index.size());
                // Add the file to archive
                outputArchive.addFile(entry.path().string(), archivePath);
                // Index path (with forward slashes as separator for consistency)
                index.push_back(
                    toForwardSlash(fs::relative(entry.path(), parentDir).string())
                );
            }
        }
    }

    // Delete previous package and create the new package
    auto outputFilepath = fs::path{outputDir} / k_packageName;
    fs::remove(outputFilepath);
    outputArchive.compressTo(outputFilepath.string());

    // Compose C++ index
    std::string idsString;
    for (uint32_t i = 0; i < static_cast<uint32_t>(index.size()); ++i) {
        idsString += std::format(k_textureIDFormatString, index[i], i);
    }
    std::string indexContents = std::format(k_indexFileFormatString, idsString);

    // Write C++ index
    std::ofstream indexFile{indexFilepath};
    indexFile.write(
        indexContents.data(), static_cast<std::streamsize>(indexContents.size())
    );
}

} // namespace re::rp
