/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <any>
#include <vector>

namespace re {

/**
 * @brief Is used for passing parameters to the next room when transitioning
 * between them.
 */
using RoomTransitionArguments = std::vector<std::any>;

} // namespace re
