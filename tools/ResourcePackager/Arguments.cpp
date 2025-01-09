/**
 *  @author    Dubsky Tomas
 */
#include <argparse/argparse.hpp>

#include <ResourcePackager/Arguments.hpp>

namespace re::rp {

CLIArguments parseArguments(int argc, char* argv[]) {
    argparse::ArgumentParser parser("ResourcePackager", "0.1.0");

    parser.add_argument("--in")
        .metavar("input_dir")
        .required()
        .append()
        .help("directory containing the input data to process");
    parser.add_argument("-o")
        .metavar("output_dir")
        .required()
        .help("directory where packaged data will be placed");
    parser.add_argument("--index")
        .metavar("output_index_file")
        .required()
        .help("filepath where C++ index file will be placed");

    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    return CLIArguments{
        .inputDirs     = parser.get<std::vector<std::string>>("--in"),
        .outputDir     = parser.get<>("-o"),
        .indexFilepath = parser.get<>("--index")
    };
}

} // namespace re::rp
