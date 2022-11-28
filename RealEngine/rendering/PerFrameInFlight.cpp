#include "PerFrameInFlight.hpp"
/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/PerFrameInFlight.hpp>

namespace RE {

int g_framesInFlight;
int g_currentFrame;
int g_nextFrame;

const int& FRAMES_IN_FLIGHT = g_framesInFlight;
const int& CURRENT_FRAME = g_currentFrame;
const int& NEXT_FRAME = g_nextFrame;

void setFramesInFlight(int framesInFLight) {
    g_framesInFlight = framesInFLight;
}

void setFrame(int totalFrame) {
    g_currentFrame = totalFrame % FRAMES_IN_FLIGHT;
    g_nextFrame = (totalFrame + 1) % FRAMES_IN_FLIGHT;
}

}
