#pragma once
#include <glm/glm.hpp>
#include "imgui/imgui.h"
#include "Utils/Time.h"
#include "Graphics/Gizmos.h"

namespace engine {
	class DebugSettings {
	public:
		static DebugSettings& Get();
		void draw();
		glm::vec4 colliderColor{ 0.f, 1.f, 0.f, 1.f };
		glm::vec4 aabbColor{ 100.f / 255.f, 100.f / 255.f, 100.f / 255.f, 1.f };
	};
}