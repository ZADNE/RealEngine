/**
 *  @author    Dubsky Tomas
 */
export module realengine.tools.resource_packager:Arguments;
import std;

export namespace re::rp {

struct CLIArguments {
    std::vector<std::string> inputDirs;
    std::string outputDir;
    std::string indexFilepath;
};

CLIArguments parseArguments(int argc, char* argv[]); // NOLINT(*-avoid-c-arrays)

} // namespace re::rp
