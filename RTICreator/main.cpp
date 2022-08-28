/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/program/MainProgram.hpp>

#include <RTICreator/MainMenuRoom.hpp>

int main(int argc, char* argv[]) {
    RE::MainProgram::initialize();
    auto* mainMenuRoom = RE::MainProgram::addRoom<MainMenuRoom>(RE::CommandLineArguments(argv, argc));
    return RE::MainProgram::run(mainMenuRoom->getName(), {});
}