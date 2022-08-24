/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/program/MainProgram.hpp>

#include <RTICreator/MainMenuRoom.hpp>

int main(int argc, char* argv[]) {
	RE::MainProgram::initialize();

	static MainMenuRoom mainMenuRoom{RE::CommandLineArguments(argv, argc), 0};

	return RE::MainProgram::run(mainMenuRoom, {});
}