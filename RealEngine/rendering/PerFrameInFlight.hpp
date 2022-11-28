/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <algorithm>

namespace RE {

extern const int& FRAMES_IN_FLIGHT;
extern const int& CURRENT_FRAME;
extern const int& NEXT_FRAME;

void setFramesInFlight(int framesInFLight);
void setFrame(int totalFrame);

constexpr int MAX_FRAMES_IN_FLIGHT = 2;
template<typename T>
struct PerFrameInFlight {

    PerFrameInFlight(std::initializer_list<T> il) :
        m_objects(il) {
    }

    PerFrameInFlight(const T& object) {
        std::fill_n(m_objects.data(), FRAMES_IN_FLIGHT, object);
    }

    T& operator[](int index) {
        return m_objects[index];
    }

    T& operator*() {
        return m_objects[CURRENT_FRAME];
    }

    T* operator->() {
        return &m_objects[CURRENT_FRAME];
    }

    std::array<T, MAX_FRAMES_IN_FLIGHT> m_objects;
};

}