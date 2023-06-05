/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/PerFrameInFlight.hpp>

namespace re {

int g_framesInFlightVar;
int g_currentFrameVar;
int g_nextFrameVar;

const int& g_framesInFlight = g_framesInFlightVar;
const int& g_currentFrame   = g_currentFrameVar;
const int& g_nextFrame      = g_nextFrameVar;

void setFramesInFlight(int framesInFLight) {
    g_framesInFlightVar = framesInFLight;
}

void setFrame(int totalFrame) {
    g_currentFrameVar = totalFrame % g_framesInFlight;
    g_nextFrameVar    = (totalFrame + 1) % g_framesInFlight;
}

} // namespace re
