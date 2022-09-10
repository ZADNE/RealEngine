/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.utility.Error;


namespace RE {

[[noreturn]] void fatalError(std::string_view error, int exitCode/* = 1*/) {
    std::cerr << error << std::endl;
    exit(exitCode);
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