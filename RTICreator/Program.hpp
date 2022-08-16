/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/main/program/MainProgram.hpp>

#include <RTICreator/MainMenuRoom.hpp>

/**
 * @brief rtiCreator program
*/
class Program : public RE::MainProgram {
public:
	Program(RE::CommandLineArguments args);

private:
	MainMenuRoom m_mainMenuRoom;
};