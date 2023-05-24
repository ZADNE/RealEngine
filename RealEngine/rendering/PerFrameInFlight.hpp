/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <algorithm>
#include <utility>
#include <initializer_list>

namespace RE {

extern const int& framesInFlight;
extern const int& currentFrame;
extern const int& nextFrame;

void setFramesInFlight(int framesInFLight);
void setFrame(int totalFrame);

constexpr int k_maxFramesInFlight = 2;
template<typename T>
using PerFrameInFlight = std::array<T, k_maxFramesInFlight>;


template<typename T>
T& current(PerFrameInFlight<T>& perFrameInFlight) {
    return perFrameInFlight[currentFrame];
}

}