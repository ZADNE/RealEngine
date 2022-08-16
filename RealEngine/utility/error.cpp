/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/utility/error.hpp>

#include <iostream>
#include <string>

namespace RE {

[[noreturn]] void fatalError(std::string_view error, int exitCode/* = EXIT_FAILURE*/) {
	std::cerr << error << std::endl;
	exit(EXIT_FAILURE);
}

void error(std::string_view error, bool appendEOL/* = true*/) {
	std::cerr << error;
	if (appendEOL) std::cerr << std::endl;
}

void log(std::string_view message, bool appendEOL/* = true*/) {
	std::cout << message;
	if (appendEOL) std::cout << '\n';
}

}