/*!
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <DataPackager/Arguments.hpp>
#include <DataPackager/Package.hpp>

int main(int argc, char* argv[]) {
    using namespace re::dp;
    try {
        CLIArguments args = parseArguments(argc, argv);
        composePackage(args.inputDirs, args.outputDir, args.indexFilepath);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
