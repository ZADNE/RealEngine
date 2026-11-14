/**
 *  @author    Dubsky Tomas
 */
export module realengine.tools.resource_packager:Package;
import std;

export namespace re::rp {

void composePackage(
    std::span<const std::string> inputDirs, const std::string& outputDir,
    const std::string& indexFilepath
);

} // namespace re::rp
