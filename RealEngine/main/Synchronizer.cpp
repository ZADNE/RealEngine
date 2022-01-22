#include <RealEngine/main/Synchronizer.hpp>

#include <thread>
#include <cassert>

using namespace std::chrono_literals;

namespace RE {

Synchronizer::Synchronizer(unsigned int stepsPerSecond, unsigned int framesPerSecondLimit, bool beginResumed/* = false*/) {
	setStepsPerSecond(stepsPerSecond);
	setFramesPerSecondLimit(framesPerSecondLimit);
	if (beginResumed) {
		resumeSteps();
	}
}

void Synchronizer::setStepsPerSecond(unsigned int stepsPerSecond) {
	assert(stepsPerSecond > 0);
	m_timePerStep = 1'000'000'000ns / stepsPerSecond;
}

void Synchronizer::setFramesPerSecondLimit(unsigned int framesPerSecondLimit) {
	m_timePerFrame = framesPerSecondLimit == DO_NOT_LIMIT_FRAMES_PER_SECOND ? Duration::zero() : 1'000'000'000ns / framesPerSecondLimit;
	resumeSteps();
}

void Synchronizer::pauseSteps() {
	m_stepsPaused = true;
}

void Synchronizer::resumeSteps() {
	m_startTime = std::chrono::steady_clock::now();
	m_lastFrameTime = m_startTime;
	m_stepTimeAccumulator = Duration::zero();
	m_currFrameIndex = 0;
	m_stepsPaused = false;
}

double Synchronizer::getDrawInterpolationFactor() const {
	return static_cast<double>(m_stepTimeAccumulator.count()) / static_cast<double>(m_timePerStep.count());
}

unsigned int Synchronizer::getFramesPerSecond() const {
	return m_framesPerSecond;
}

Synchronizer::Duration Synchronizer::getMaxFrameTime() const {
	return m_maxFrameTime;
}

void Synchronizer::beginFrame() {
	auto now = std::chrono::steady_clock::now();
	auto frameTime = now - m_lastFrameTime;
	m_stepTimeAccumulator += frameTime;//Accumulate time of last frame

	auto nowSec = std::chrono::time_point_cast<std::chrono::seconds>(now);
	auto lastFrameTimeSec = std::chrono::time_point_cast<std::chrono::seconds>(m_lastFrameTime);
	if (nowSec > lastFrameTimeSec) {//If this frames begins at next second
		//Save statistics of the last second
		m_framesPerSecond = m_framesPerSecondThisSecond;
		m_framesPerSecondThisSecond = 0;
		m_maxFrameTime = m_maxFrameTimeThisSecond;
		m_maxFrameTimeThisSecond = frameTime;
	}

	//Update statistics of this second
	m_framesPerSecondThisSecond++;
	if (frameTime > m_maxFrameTimeThisSecond) {
		m_maxFrameTimeThisSecond = frameTime;//Update max frame time if it is the highest
	}

	m_lastFrameTime = now;
}

void Synchronizer::endFrame() {
	if (m_timePerFrame != Duration::zero()) {//If frames per second should be limited
		delayTillEndOfFrame();
	}
	m_currFrameIndex++;
}

bool Synchronizer::shouldStepHappen() {
	if (m_stepsPaused) {
		return false;
	}

	if (m_stepTimeAccumulator >= m_timePerStep) {//If accumulated enough time for the next step to happen
		m_stepTimeAccumulator -= m_timePerStep;//Reduce accumulated time
		return true;
	} else {
		return false;
	}
}

void Synchronizer::delayTillEndOfFrame() {
	auto now = std::chrono::steady_clock::now();
	auto expectedFrameEnd = m_startTime + m_currFrameIndex * m_timePerFrame;
	if (now < expectedFrameEnd) {//If there is time to sleep
		std::this_thread::sleep_until(expectedFrameEnd);//Sleep   zzZZZzz
	}
}

}