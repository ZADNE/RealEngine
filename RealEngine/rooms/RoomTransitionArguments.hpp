/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>
#include <any>

namespace RE {

/**
 * @brief Used for passing parameters to the next room when transitioning between them.
*/
using RoomTransitionArguments = std::vector<std::any>;

}