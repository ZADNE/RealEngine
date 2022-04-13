﻿#pragma once
#include <RealEngine/main/MainProgram.hpp>

#include <RTICreator/MainMenuRoom.hpp>

/**
 * @brief rtiCreator program
*/
class Program : public RE::MainProgram {
public:
	Program(RE::CommandLineArguments args);
	~Program();
private:
	MainMenuRoom m_mainMenuRoom;
};