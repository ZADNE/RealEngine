/*!
 *  @author    Dubsky Tomas
 */
#include <iostream>
#include <string>

#include <RealEngine/utility/Error.hpp>

namespace re {

[[noreturn]] void fatalError(std::string_view error, int exitCode /* = EXIT_FAILURE*/) {
    std::cerr << error << std::endl;
    exit(EXIT_FAILURE);
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