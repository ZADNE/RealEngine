/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <initializer_list>
#include <utility>

namespace re {

/**
 * @brief RealEngine supports at most double buffering (triple is not supported)
 */
constexpr int k_maxFramesInFlight = 2;

class DoubleBufferingState {
public:
    DoubleBufferingState() {}

    void setTotalIndex(int totalIndex) {
        m_writeIndex = totalIndex % 2;
        m_readIndex  = (totalIndex + 1) % 2;
    }

    int writeIndex() const { return m_writeIndex; }
    int readIndex() const { return m_readIndex; }

private:
    int m_writeIndex = 0;
    int m_readIndex  = 1;
};

/**
 * @brief Represents current state for frame-wise double buffering
 * @note The state is updated automatically by RealEngine, do not update
 * it manually.
 */
extern const DoubleBufferingState& g_frameDoubleBufferingState;

/**
 * @brief Represents current state for step-wise double buffering
 * @note The state is updated automatically by RealEngine, do not update
 * it manually.
 */
extern const DoubleBufferingState& g_stepDoubleBufferingState;

/**
 * @brief Helps with managing double buffered objects
 */
template<typename T, const DoubleBufferingState* state>
class DoubleBuffered {
    template<class Tother, const DoubleBufferingState*>
    friend class DoubleBuffered;

public:
    DoubleBuffered() {}
    DoubleBuffered(T&& first, T&& second)
        : m_ts{std::move(first), std::move(second)} {}

    T&       write() { return m_ts[state->writeIndex()]; }
    const T& read() const { return m_ts[state->readIndex()]; }

    void forEach(std::invocable<T&> auto func) {
        for (auto& t : m_ts) { func(t); }
    }

    template<typename Arg>
    void forEach(
        std::invocable<T&, const Arg&> auto func,
        const DoubleBuffered<Arg, state>&   arg
    ) {
        for (auto it = std::make_pair(m_ts.begin(), arg.m_ts.cbegin());
             it.first != m_ts.end();
             ++it.first, ++it.second) {
            func(*it.first, *it.second);
        }
    }

private:
    std::array<T, 2> m_ts;
};

/**
 * @brief Represents frame-wise double buffered object
 */
template<typename T>
using FrameDoubleBuffered = DoubleBuffered<T, &g_frameDoubleBufferingState>;

/**
 * @brief Represents step-wise double buffered object
 */
template<typename T>
using StepDoubleBuffered = DoubleBuffered<T, &g_stepDoubleBufferingState>;

namespace details {
extern DoubleBufferingState& g_frameDoubleBufferingState;
extern DoubleBufferingState& g_stepDoubleBufferingState;
} // namespace details

} // namespace re
