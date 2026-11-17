/**
 *  @author    Dubsky Tomas
 */
module realengine.tools.resource_packager:Main_impl;
import :Arguments;
import :Package;

extern "C++" int main(int argc, char* argv[]) {
    using namespace re::resp;
    try {
        CLIArguments args = parseArguments(argc, argv);
        composePackage(
            args.inputDirs, args.outputDir, args.indexFilepath, args.moduleName
        );
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
