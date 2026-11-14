/**
 *  @author    Dubsky Tomas
 */
module realengine.tools.rti_creator:Main_impl;
import realengine;
import :MainMenuRoom;

extern "C++" int main(int argc, char* argv[]) {
    re::MainProgram::initialize({});
    auto* mainMenuRoom = re::MainProgram::addRoom<MainMenuRoom>(
        re::CommandLineArguments(const_cast<const char**>(argv), argc)
    );
    return re::MainProgram::run(mainMenuRoom->name(), {});
}
