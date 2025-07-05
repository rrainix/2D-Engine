#pragma once
#include <GLFW/glfw3.h>
#include <chrono>

namespace engine {
	class Time {
	public:
		static float deltaTime() { return s_deltaTime * s_timeScale; }
		static float unscaledDeltaTime() { return s_deltaTime; }
		static void targetFramerate(float fps);
		static float targetFramerate() { return s_targetFPS; }

		static float fixedDeltaTime() { return s_fixedDeltaTime * s_timeScale; }
		static void fixedDeltaTime(float step);

		static float unscaledFixedDeltaTime() { return s_fixedDeltaTime; }
		static float getMaxPossibleFPS(){ return s_maxPossibleFPS; }

		static float timeScale() { return s_timeScale; }
		static void timeScale(float scale);

		/* Realtime elapsed time */
		static float elapsedTime();
		/* Elapsed time based on timescale*/
		static float simulatedElapsedTime();

	private:
		static void update(float deltaTime);
		static float s_deltaTime;
		static float s_targetFPS;
		static float s_timeScale;
		static float s_updateDeltaTime;
		static float s_fixedDeltaTime;
		static float s_simulatedElapsedTime;
		static int s_frameCount;
		static float s_maxPossibleFPS;

		static std::chrono::steady_clock::duration s_frameDuration;
		static std::chrono::high_resolution_clock::time_point s_startTime;

		friend class Application;
	};
}