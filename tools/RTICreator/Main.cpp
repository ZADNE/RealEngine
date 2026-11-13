/**
 *  @author    Dubsky Tomas
 */
module rticreator:Main;
import realengine;
import :MainMenuRoom;

extern "C++" int main(int argc, char* argv[]) {
    re::MainProgram::initialize({});
    auto* mainMenuRoom = re::MainProgram::addRoom<MainMenuRoom>(
        re::CommandLineArguments(const_cast<const char**>(argv), argc)
    );
    return re::MainProgram::run(mainMenuRoom->name(), {});
}
