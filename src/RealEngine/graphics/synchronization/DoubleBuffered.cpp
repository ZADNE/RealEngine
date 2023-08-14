/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>

namespace re {

DoubleBufferingState g_frameDoubleBufferingStateVar;
DoubleBufferingState g_stepDoubleBufferingStateVar;

const DoubleBufferingState& g_frameDoubleBufferingState = g_frameDoubleBufferingStateVar;
const DoubleBufferingState& g_stepDoubleBufferingState = g_stepDoubleBufferingStateVar;

namespace details {
DoubleBufferingState& g_frameDoubleBufferingState = g_frameDoubleBufferingStateVar;
DoubleBufferingState& g_stepDoubleBufferingState = g_stepDoubleBufferingStateVar;
} // namespace details

} // namespace re
