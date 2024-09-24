/**
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

template<typename T>
concept DoubleBufferingState = requires(T) {
    { T::writeIndex() } -> std::convertible_to<int>;
    { T::readIndex() } -> std::convertible_to<int>;
};

/**
 * @brief Helps with managing double buffered objects
 */
template<typename T, DoubleBufferingState State>
class DoubleBuffered {
    template<class Tother, DoubleBufferingState>
    friend class DoubleBuffered;

public:
    using Type = T;

    DoubleBuffered() {}
    DoubleBuffered(T&& first, T&& second)
        : m_ts{std::move(first), std::move(second)} {}

    /**
     * @brief Accesses the object that should be used for writing from CPU
     */
    const T& write() const { return m_ts[State::writeIndex()]; }
    T& write() { return m_ts[State::writeIndex()]; }
    const T& operator*() const { return write(); }
    const T* operator->() const { return &write(); }
    T& operator*() { return write(); }
    T* operator->() { return &write(); }

    const T& read() const { return m_ts[State::readIndex()]; }
    T& read() { return m_ts[State::readIndex()]; }

    /**
     * @brief Calls 'func' on each buffer
     */
    void forEach(std::invocable<T&> auto func) {
        for (auto& t : m_ts) { func(t); }
    }

    /**
     * @brief Calls 'func' on each buffer of this and 'arg'
     */
    template<typename Arg>
    void forEach(
        std::invocable<T&, const Arg&> auto func,
        const DoubleBuffered<Arg, State>& arg
    ) {
        for (auto it = std::make_pair(m_ts.begin(), arg.m_ts.cbegin());
             it.first != m_ts.end(); ++it.first, ++it.second) {
            func(*it.first, *it.second);
        }
    }

    T& operator[](int i) { return m_ts[i]; }

private:
    std::array<T, 2> m_ts;
};

class FrameDoubleBufferingState {
public:
    // Updated internally by RealEngine
    static void setTotalIndex(int totalIndex) {
        s_writeIndex = totalIndex % 2;
        s_readIndex  = (totalIndex + 1) % 2;
    }

    static int writeIndex() { return s_writeIndex; }
    static int readIndex() { return s_readIndex; }

private:
    static inline int s_writeIndex = 0;
    static inline int s_readIndex  = 1;
};
static_assert(DoubleBufferingState<FrameDoubleBufferingState>);

class StepDoubleBufferingState {
public:
    // Updated internally by RealEngine
    static void setTotalIndex(int totalIndex) {
        s_writeIndex = totalIndex % 2;
        s_readIndex  = (totalIndex + 1) % 2;
    }

    static int writeIndex() { return s_writeIndex; }
    static int readIndex() { return s_readIndex; }

private:
    static inline int s_writeIndex = 0;
    static inline int s_readIndex  = 1;
};
static_assert(DoubleBufferingState<StepDoubleBufferingState>);

/**
 * @brief Represents frame-wise double buffered object
 */
template<typename T>
using FrameDoubleBuffered = DoubleBuffered<T, FrameDoubleBufferingState>;

/**
 * @brief Represents step-wise double buffered object
 */
template<typename T>
using StepDoubleBuffered = DoubleBuffered<T, StepDoubleBufferingState>;

} // namespace re
