﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/PerFrameInFlight.hpp>

namespace RE {

int g_framesInFlight;
int g_currentFrame;
int g_nextFrame;

const int& framesInFlight = g_framesInFlight;
const int& currentFrame = g_currentFrame;
const int& nextFrame = g_nextFrame;

void setFramesInFlight(int framesInFLight) {
    g_framesInFlight = framesInFLight;
}

void setFrame(int totalFrame) {
    g_currentFrame = totalFrame % framesInFlight;
    g_nextFrame = (totalFrame + 1) % framesInFlight;
}

}
