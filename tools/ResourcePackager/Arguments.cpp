/**
 *  @author    Dubsky Tomas
 */
module realengine.tools.resource_packager:Arguments_impl;
import std;
import argparse;
import :Arguments;

namespace re::rp {

CLIArguments parseArguments(int argc, char* argv[]) { // NOLINT(*-avoid-c-arrays)
    argparse::ArgumentParser parser("ResourcePackager", "0.1.1");

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
        parser.parse_args(std::vector<std::string>{&argv[0], &argv[argc]});
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
