/*!
 *  @author    Dubsky Tomas
 */
#include <RealShadersGenerator/Arguments.hpp>
#include <RealShadersGenerator/Utility.hpp>
#include <argparse/argparse.hpp>

namespace rsg {

CLIArguments parseArguments(int argc, char* argv[]) {
    argparse::ArgumentParser parser("RealShadersGenerator", "0.1.0");

    parser.add_argument("-I").append().help("Include directory");
    parser.add_argument("input file").help("Input filename");
    parser.add_argument("-o").append().help("Output filename");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    return CLIArguments{
        .inputFile   = parser.get<>("input file"),
        .outputFile  = parser.get<>("-o"),
        .includeDirs = parser.get<std::vector<std::string>>("-I")
    };
}

} // namespace rsg
