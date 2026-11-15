/**
 *  @author    Dubsky Tomas
 */
export module realengine:rooms.RoomTransitionArguments;
import std;

export namespace re {

/**
 * @brief Is used for passing parameters to the next room when transitioning
 * between them.
 */
using RoomTransitionArguments = std::vector<std::any>;

} // namespace re
