/*!
 *  @author    Dubsky Tomas
 */
#include <argparse/argparse.hpp>

#include <DataPackager/Arguments.hpp>

namespace re::dp {

CLIArguments parseArguments(int argc, char* argv[]) {
    argparse::ArgumentParser parser("DataPackager", "0.1.0");

    parser.add_argument("--in")
        .metavar("input_dir")
        .required()
        .append()
        .help("directory containing the input data to process");
    parser.add_argument("-o")
        .metavar("output_dir")
        .required()
        .help("directory where packaged data will be placed");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    return CLIArguments{
        .inputDirs = parser.get<std::vector<std::string>>("--in"),
        .outputDir = parser.get<>("-o")
    };
}

} // namespace re::dp
