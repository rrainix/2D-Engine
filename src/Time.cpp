#include "Time.h"
#include <algorithm>
#include <iostream>

namespace engine {

	float Time::s_timeScale = 1.f;
	float Time::s_deltaTime = 0.0f;
	float Time::s_simulatedElapsedTime = 0.0f;
	int Time::s_frameCount = 0;

	float Time::s_fixedDeltaTime = 1.0f / 50.f;
	float Time::s_targetFPS = 0.f;
	float Time::s_maxPossibleFPS = 0.f;
	float Time::s_updateDeltaTime = 1.0f / s_targetFPS;

	std::chrono::steady_clock::duration Time::s_frameDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
		std::chrono::duration<float>(1.0f / s_targetFPS)
	);
	std::chrono::high_resolution_clock::time_point Time::s_startTime = std::chrono::high_resolution_clock::now();

	void Time::targetFramerate(float framerate) {
		s_targetFPS = framerate;
		if (s_targetFPS > 0)
			s_updateDeltaTime = 1.0f / s_targetFPS;
		else
			s_updateDeltaTime = std::numeric_limits<float>::infinity();

		s_frameDuration =
			std::chrono::duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<float>(1.0f / s_targetFPS)
			);
	}

	void Time::timeScale(float scale) {
		s_timeScale = scale;
	}

	void Time::fixedDeltaTime(float step) {
		step = std::clamp(step, 0.f, 1.f);
		s_fixedDeltaTime = step;
	}

	float Time::elapsedTime() {
		std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - s_startTime;
		return elapsed.count();
	}

	float Time::simulatedElapsedTime() {	return s_simulatedElapsedTime; }

	void Time::update(float deltaTime) {
		 Time::s_deltaTime = deltaTime;
		 s_simulatedElapsedTime += Time::s_deltaTime * s_timeScale;
		 s_frameCount++;
	}
}