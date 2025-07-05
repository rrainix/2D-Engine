#pragma once
#include "Window.h"
#include "Graphics/RenderSystem.h"
#include "Utils/Input.h"
#include "Utils/Time.h"
#include "Utils/PerlinNoise.h"
#include <chrono>
#include "Scene.h"
#include "SceneManager.h"
#include <thread>
#include "Experimental/GameSystem.h"
#include "Utils/Debug.h"
#include "Components/Transform.h"
#include "DebugWindow.h"
#include "Profiler.h"

namespace engine {
	class Application {
	public:
		Application(Window& window);
		~Application();
		void run();

	private:
		void initImGUI();
		void destroyImGUI();

		Window& m_window;
		graphics::RenderSystem m_renderSystem;
		PhysicsSystem m_physicsSystem;
	};
}