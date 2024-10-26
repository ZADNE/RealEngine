/*!
 *  @author    Dubsky Tomas
 */
#include <RealShadersGenTool/Arguments.hpp>
#include <RealShadersGenTool/Utility.hpp>
#include <argparse/argparse.hpp>

namespace rsg {

CLIArguments parseArguments(int argc, char* argv[]) {
    argparse::ArgumentParser parser("RealShadersGenTool", "0.1.0");

    parser.add_argument("-I").append().metavar("dir").help("include directory");
    parser.add_argument("--namespace")
        .metavar("namespace")
        .help("namespace to generate the reflection to");
    parser.add_argument("-o").metavar("outfile").required().help(
        "output C++ file"
    );
    parser.add_argument("infile").help(
        "input GLSL file, "
        "name of the interface block is the same as the filename"
    );

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    return CLIArguments{
        .inputFile   = parser.get<>("infile"),
        .outputFile  = parser.get<>("-o"),
        .includeDirs = parser.get<std::vector<std::string>>("-I"),
        .namespace_  = parser.present<>("--namespace").value_or("")
    };
}

} // namespace rsg
