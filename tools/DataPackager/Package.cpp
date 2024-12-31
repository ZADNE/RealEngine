/*!
 *  @author    Dubsky Tomas
 */
#include <filesystem>

#include <bit7z/bitfilecompressor.hpp>

#include <RealEngine/resources/PackageLoader.hpp>

#include <DataPackager/Package.hpp>

namespace re::dp {

void composeTileItemTextures(
    std::span<const std::string> inputDirs, const std::string& outputDir
) {
    auto outputFile = std::filesystem::path{outputDir} / PackageLoader::k_packageName;
    // Delete previous package (if present)
    std::filesystem::remove(outputFile);

    // Create new package
    bit7z::Bit7zLibrary lib{};
    bit7z::BitFileCompressor compressor{lib, bit7z::BitFormat::SevenZip};
    compressor.setPassword(PackageLoader::k_packageKey, true);
    bit7z::BitOutputArchive outputArchive{compressor};
    for (const std::string& inputDir : inputDirs) {
        outputArchive.addDirectory(inputDir);
    }
    outputArchive.compressTo(outputFile.string());
}

} // namespace re::dp
