/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <algorithm>
#include <utility>
#include <initializer_list>

namespace RE {

extern const int& FRAMES_IN_FLIGHT;
extern const int& CURRENT_FRAME;
extern const int& NEXT_FRAME;

void setFramesInFlight(int framesInFLight);
void setFrame(int totalFrame);

constexpr int MAX_FRAMES_IN_FLIGHT = 2;
template<typename T>
using PerFrameInFlight = std::array<T, MAX_FRAMES_IN_FLIGHT>;


template<typename T>
T& current(PerFrameInFlight<T>& perFrameInFlight) {
    return perFrameInFlight[CURRENT_FRAME];
}

}