/*! 
 *  @author    Dubsky Tomas
 */
#include "Program.hpp"


Program::Program(RE::CommandLineArguments args) :
	m_mainMenuRoom(args) {
	p_window.setTitle("RTICreator v3.0.0");

	auto mainRoom = p_roomManager.addRoom(&m_mainMenuRoom);

	p_roomManager.gotoRoom(mainRoom, {});
}

Program::~Program() {

}
