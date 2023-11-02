/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/program/MainProgram.hpp>

#include <RTICreator/MainMenuRoom.hpp>

int main(int argc, char* argv[]) {
    re::MainProgram::initialize({});
    auto* mainMenuRoom = re::MainProgram::addRoom<MainMenuRoom>(
        re::CommandLineArguments(argv, argc)
    );
    return re::MainProgram::run(mainMenuRoom->name(), {});
}