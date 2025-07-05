#pragma once
#include "Application.h"
#include <windows.h>
#include <immintrin.h>
#include "Window.h"
#include "Experimental/CameraSystem.h"
#include "Utils/keygen.h"
#include <psapi.h>


namespace engine {

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

	size_t getMemoryUsageInMB() {
#if defined(_WIN32)
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
			return pmc.WorkingSetSize / (1024 * 1024); // in MB
		}
		return 0;

#elif defined(__linux__)
		struct rusage usage;
		if (getrusage(RUSAGE_SELF, &usage) == 0) {
			return usage.ru_maxrss / 1024; // Linux: ru_maxrss in KB → MB
		}
		return 0;

#elif defined(__APPLE__)
		struct rusage usage;
		if (getrusage(RUSAGE_SELF, &usage) == 0) {
			return usage.ru_maxrss / (1024 * 1024); // macOS: ru_maxrss in Bytes → MB
		}
		return 0;

#else
		return 0; // Unsupported platform
#endif
	}
	size_t getHeapUsageBytes() {
#if defined(_WIN32)
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		return static_cast<size_t>(pmc.PrivateUsage); // Only heap-like private memory

#elif defined(__linux__)
		std::ifstream statm("/proc/self/statm");
		size_t totalPages = 0;
		statm >> totalPages;
		return totalPages * sysconf(_SC_PAGESIZE);

#elif defined(__APPLE__)
		struct task_basic_info t_info;
		mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
		task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
		return t_info.resident_size;

#else
		return 0; // Unsupported platform
#endif
	}
	size_t getStackUsageBytes() {
#if defined(_WIN32)
		NT_TIB* tib = (NT_TIB*)NtCurrentTeb();
		uintptr_t stackBase = (uintptr_t)tib->StackBase;
		uintptr_t stackCurrent = (uintptr_t)&tib;
		return stackBase - stackCurrent;

#elif defined(__linux__)
		pthread_attr_t attr;
		pthread_getattr_np(pthread_self(), &attr);
		void* stackBase;
		size_t stackSize;
		pthread_attr_getstack(&attr, &stackBase, &stackSize);
		pthread_attr_destroy(&attr);
		uintptr_t sp = (uintptr_t)__builtin_frame_address(0);
		return (uintptr_t)stackBase + stackSize - sp;

#elif defined(__APPLE__)
		void* stackBase = pthread_get_stackaddr_np(pthread_self());
		size_t stackSize = pthread_get_stacksize_np(pthread_self());
		uintptr_t sp = (uintptr_t)__builtin_frame_address(0);
		return (uintptr_t)stackBase - sp;

#else
		return 0; // Unsupported platform
#endif
	}

	Application::Application(Window& window) : m_window{ window } {
		initImGUI();
		m_renderSystem.init();
		Input::s_window = m_window.glfwWindow();
	}

	void Application::initImGUI() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfw_InitForOpenGL(m_window.glfwWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
		ImGui::StyleColorsDark();
	}

	void Application::destroyImGUI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	Application::~Application() {
		destroyImGUI();
		m_renderSystem.destroy();
	}

	void Application::run() {
		auto nextFrameTime = std::chrono::steady_clock::now();
		const auto     spinThreshold = std::chrono::microseconds(1500);
		auto lastFrameTime = std::chrono::high_resolution_clock::now();
		float fixedUpdateAccumulator = 0.0f;

		timeBeginPeriod(1);

		float timeScale = 1.f;

		while (!m_window.shouldClose()) {
			Profiler::Get().BeginFrame();

			SET_MEM_STAT("Total allocated memory", std::to_string(getMemoryUsageInMB()) + "MB");
			SET_MEM_STAT("Heap", std::to_string(getHeapUsageBytes() / (1024 * 1024)) + "MB");
			SET_MEM_STAT("Stack", std::to_string(getStackUsageBytes()) + " Bytes");

			SET_GEN_STAT("FPS", std::to_string(1.f / Time::unscaledDeltaTime()));
			SET_GEN_STAT("Possible FPS", std::to_string(1.f / Time::getMaxPossibleFPS()));
			SET_CPU_STAT("Main thread", std::to_string(Time::deltaTime()) + " ms");
			SET_GEN_STAT("Elapsed time", std::to_string(Time::elapsedTime()));

			auto frameStart = std::chrono::steady_clock::now();
			float deltaSeconds = std::chrono::duration<float>(frameStart - lastFrameTime).count();
			lastFrameTime = frameStart;
			engine::Time::update(deltaSeconds);

			if (m_window.isWindowResized()) {
				glm::ivec2 size = m_window.size();
				graphics::Camera& mainCam = *graphics::Camera::main();
				mainCam.updateViewSize(size.x, size.y);
				mainCam.updateProjection();
				m_window.resetWindowResizedFlag();
			}



			fixedUpdateAccumulator += deltaSeconds * Time::s_timeScale;
			while (fixedUpdateAccumulator >= Time::s_fixedDeltaTime) {
				SceneManager::fixedUpdateScenes();

				{
					PROFILE_CPU("Physics", Profiler::CPUCategory::Physics);
					try {
						m_physicsSystem.update();
					}
					catch (std::runtime_error e) {
						Debug::logError(e.what());
					}
				}

				fixedUpdateAccumulator -= Time::s_fixedDeltaTime;
			}

			Input::updateKeyStates();
			glClear(GL_COLOR_BUFFER_BIT);


			SceneManager::updateScenes();

			{
				PROFILE_CPU("Rendering", Profiler::CPUCategory::Rendering);
				try {
					m_renderSystem.update();
				}
				catch (std::runtime_error e) {
					Debug::logError(e.what());
				}
			}


			Profiler::Get().EndFrame();
			m_window.swapBuffers();
			glfwPollEvents();

			Time::s_maxPossibleFPS = std::chrono::duration<float>(std::chrono::steady_clock::now() - frameStart).count();;

			nextFrameTime += Time::s_frameDuration;

			auto now = std::chrono::steady_clock::now();
			if (nextFrameTime < now)
				nextFrameTime = now;


			auto sleepTime = nextFrameTime - now;
			if (sleepTime > spinThreshold) {
				std::this_thread::sleep_for(sleepTime - spinThreshold);
			}
			while (std::chrono::steady_clock::now() < nextFrameTime) {
				_mm_pause();
			}
		}
		timeEndPeriod(1);
	}
}