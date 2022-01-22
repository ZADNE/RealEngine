#pragma once
#include <chrono>

namespace RE {

class MainProgram;

/**
 * @brief Synchronizer ensures that steps happen at a constant rate per second.
 * Optionally, limits the number of frames per second.
 *
 * To be used in conjunction with MainProgram.
*/
class Synchronizer {
	friend class MainProgram;
public:
	using TimePoint = std::chrono::steady_clock::time_point;/**< Time point type alias */
	using Duration = std::chrono::steady_clock::duration;/**< Time duration type alias */

	const static unsigned int DO_NOT_LIMIT_FRAMES_PER_SECOND = 0u;/**< Constant which denotes that frames per second should not be limited */

	/**
	 * @brief Constructs new synchronizer.
	 *
	 * @param stepsPerSecond Number of steps per second, steps will happen at this constant rate per second
	 * @param framesPerSecondLimit	Maximum number of frames per second (frame draws will happen at most at this constant rate per second),
	 * 								or use DO_NOT_LIMIT_FRAMES_PER_SECOND to not limit frames per second.
	 * @param beginResumed If true, steps will be resumed from the beginning.
	*/
	Synchronizer(unsigned int stepsPerSecond, unsigned int framesPerSecondLimit, bool beginResumed = false);

	/**
	 * @brief Sets desired number of steps to happen per second.
	 *
	 * Zero is not a valid value - use pauseSteps() to stop steps.
	 * @param stepsPerSecond Desired number of steps to happen per second.
	*/
	void setStepsPerSecond(unsigned int stepsPerSecond);

	/**
	 * @brief Sets new limit for frames to be drawn per second.
	 * Or use DO_NOT_LIMIT_FRAMES_PER_SECOND to not limit frames per second.
	 *
	 * @param framesPerSecondLimit	Maximum number of frames to be drawn per second,
	 * 								or use DO_NOT_LIMIT_FRAMES_PER_SECOND to not limit this
	*/
	void setFramesPerSecondLimit(unsigned int framesPerSecondLimit);

	/**
	 * @brief Temporarily pauses steps.
	 * No steps will be reported to happen until resumeSteps() is called.
	 * Use this if you need to perform a time sensitive operation that will greatly
	 * exceed the time of one step.
	*/
	void pauseSteps();

	/**
	 * @brief Resumes temporarily paused steps.
	 * Steps will resume as if they were not paused at all
	 * (that is without rushing to compensate for the time of the pause).
	*/
	void resumeSteps();

	/**
	 * @brief Gets interpolation factor for drawing of the next frame.
	 *
	 * Interpolation factor represent relative time between two steps.
	 * So the return value is always in range <0, 1), where 0 means that the draw happens right after
	 * a step and value close to 1 means that it happens close to the next step, linearly.
	 * @return Interpolation factor of the next frame.
	*/
	double getDrawInterpolationFactor() const;

	/**
	 * @brief Gets number of frames drawn last second (usually refered to as FPS).
	 * This is the most basic metric for any real-time application.
	 * @return Current frames per second
	*/
	unsigned int getFramesPerSecond() const;

	/**
	 * @brief Gets maximum frame time in the last second
	 * This metric can be used to detect lag spikes.
	 * @return Maximum frame time in the last second
	*/
	Duration getMaxFrameTime() const;
private:

	/**
	 * @brief This function has to be called at the beginning of each frame
	*/
	void beginFrame();

	/**
	 * @brief This function has to be called at the end of each frame
	*/
	void endFrame();

	/**
	 * @brief Tests whether next step should happen - that depends on the time that passed since the last step.
	 *
	 * If it should, it assumes it will happen immediately, and it updates internal counters.
	 * @return True if step should happen immediately, false otherwise.
	*/
	bool shouldStepHappen();

	/**
	 * @brief Sleeps this thread until next frame should begin.
	*/
	void delayTillEndOfFrame();

	unsigned int m_currFrameIndex = 0;	/**< Number of frames drawn since last resume, used when limiting frames per second */
	TimePoint m_startTime;				/**< Time point of the last resume - the center synchronization point */
	TimePoint m_lastFrameTime;			/**< Time point of the last frame start - used to determine its period */

	Duration m_timePerStep;				/**< Time duration for each step */
	Duration m_timePerFrame;			/**< Time duration for each frame, zero if not limiting FPS (time per frame can vary) */

	Duration m_stepTimeAccumulator{Duration::zero()};/**< Accumulator of time for next step */

	unsigned int m_framesPerSecond = 0;				/**< Frames drawn last second */
	unsigned int m_framesPerSecondThisSecond = 0;	/**< Frames drawn this second */

	Duration m_maxFrameTime{Duration::zero()};		/**< Max frame time last second */
	Duration m_maxFrameTimeThisSecond{Duration::zero()};/**< Max frame time this second */

	bool m_stepsPaused = false;/**< True if steps are paused */
};

}
