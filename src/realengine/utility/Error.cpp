/**
 *  @author    Dubsky Tomas
 */
module realengine:utility.Error_impl;
import std;
import :utility.Error;

namespace re {

void fatalError(std::string_view error, int exitCode /* = 1*/) {
    std::cerr << error << std::endl;
    std::exit(exitCode);
}

void error(std::string_view error, bool appendEOL /* = true*/) {
    std::cerr << error;
    if (appendEOL)
        std::cerr << std::endl;
}

void log(std::string_view message, bool appendEOL /* = true*/) {
    std::cout << message;
    if (appendEOL)
        std::cout << '\n';
}

} // namespace re
