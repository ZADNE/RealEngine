/*! 
 *  @author    Dubsky Tomas
 */
#include "Program.hpp"


Program::Program(RE::CommandLineArguments args) :
	m_mainMenuRoom(args) {
	m_window.setTitle("RTICreator v3.0.0");

	auto mainRoom = m_roomManager.addRoom(&m_mainMenuRoom);

	m_roomManager.gotoRoom(mainRoom, {});
}
