/**
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <ResourcePackager/Arguments.hpp>
#include <ResourcePackager/Package.hpp>

int main(int argc, char* argv[]) { // NOLINT(*-avoid-c-arrays)
    using namespace re::rp;
    try {
        CLIArguments args = parseArguments(argc, argv);
        composePackage(args.inputDirs, args.outputDir, args.indexFilepath);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
