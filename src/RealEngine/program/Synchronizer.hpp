/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <chrono>

namespace re {

class MainProgram;

/**
 * @brief Ensures that steps happen at a constant rate per second.
 *
 * Optionally, limits the number of frames per second.
 */
class Synchronizer {
public:
    using TimePoint = std::chrono::steady_clock::time_point; ///< Time point type alias
    using Duration = std::chrono::steady_clock::duration; ///< Time duration type alias

    constexpr static unsigned int k_doNotLimitFramesPerSecond = 0u;

    /**
     * @brief Constructs new synchronizer.
     *
     * @param stepsPerSecond        Number of steps per second, steps will
     * happen at this constant rate per second
     * @param framesPerSecondLimit  Maximum number of frames per second (frame
     * draws will happen at most at this constant rate per second), or use
     * k_doNotLimitFramesPerSecond to not limit frames per second.
     * @param beginResumed          If true, steps will be resumed from the
     * beginning.
     */
    Synchronizer(
        unsigned int stepsPerSecond, unsigned int framesPerSecondLimit,
        bool beginResumed = false
    );

    /**
     * @brief Sets desired number of steps to happen per second.
     *
     * @param stepsPerSecond Desired number of steps to happen per second. Zero
     * is not a valid value - use pauseSteps() to stop steps.
     */
    void setStepsPerSecond(unsigned int stepsPerSecond);

    /**
     * @brief Sets new limit for frames to be drawn per second.
     *
     * @param framesPerSecondLimit  Maximum number of frames to be drawn per
     * second, or use k_doNotLimitFramesPerSecond to not limit this
     */
    void setFramesPerSecondLimit(unsigned int framesPerSecondLimit);

    /**
     * @brief Temporarily pauses steps.
     *
     * No steps will be reported to happen until resumeSteps() is
     * called. Use this if you need to perform a time sensitive operation that
     * will greatly exceed the time of one step.
     */
    void pauseSteps();

    /**
     * @brief Resumes temporarily paused steps.
     *
     * Steps will resume as if they were not paused at all
     * (that is without rushing to compensate for the time of the pause).
     */
    void resumeSteps();

    /**
     * @brief Gets interpolation factor for drawing of the next frame.
     *
     * Interpolation factor represent relative time between two steps.
     * So the return value is always in range <0, 1), where 0 means that the
     * draw happens right after a step and value close to 1 means that it
     * happens close to the next step, linearly.
     * @return Interpolation factor of the next frame.
     */
    double drawInterpolationFactor() const;

    /**
     * @brief Gets number of frames drawn last second (aka FPS).
     *
     * This is the most basic metric for any real-time application.
     * @return Current frames per second
     */
    unsigned int framesPerSecond() const;

    /**
     * @brief Gets maximum frame time in the last second
     *
     * This metric can be used to detect lag spikes.
     * @return Maximum frame time in the last second
     */
    Duration maxFrameTime() const;

    /** @brief This function has to be called at the beginning of each frame */
    void beginFrame();

    /** @brief This function has to be called at the end of each frame */
    void endFrame();

    /**
     * @brief Tests whether next step should happen.
     * @return True if step should happen immediately, false otherwise.
     *
     * Result depends on the time that passed since the last step.
     * If it should, it assumes it will happen immediately, and it updates
     * internal counters.
     */
    bool shouldStepHappen();

private:

    /** @brief Sleeps this thread until next frame should begin. */
    void delayTillEndOfFrame() const;

    void resetSynchronization();

    /// Number of frames drawn since last resume, used when limiting frames per second
    unsigned int m_currFrameIndex = 0;
    /// Time point of the last resume - the center synchronization point
    TimePoint m_startTime;
    /// Time point of the last frame start - used to determine its period
    TimePoint m_lastFrameTime;

    /// Time duration for each step
    Duration m_timePerStep{};
    /// Time duration for each frame, zero if not limiting FPS (time per frame can vary)
    Duration m_timePerFrame{};

    /// Accumulator of time for next step
    Duration m_stepTimeAccumulator{Duration::zero()};

    unsigned int m_framesPerSecond           = 0; ///< Count drawn last second
    unsigned int m_framesPerSecondThisSecond = 0; ///< Count drawn this second

    Duration m_maxFrameTime{Duration::zero()};    ///< Max last second
    Duration m_maxFrameTimeThisSecond{Duration::zero()}; ///< Max this second

    bool m_stepsPaused = false; ///< True if steps are paused
};

} // namespace re
