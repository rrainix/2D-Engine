#include "DebugSettings.h"

namespace engine {
	DebugSettings& DebugSettings::Get() {
		static DebugSettings debugSettings;
		return debugSettings;
	}


	void DebugSettings::draw() {
		ImGui::Begin("Debug Settings");

		static float targetFrameRate = 144;
		ImGui::SliderFloat("Target FPS", &targetFrameRate, 0.f, 244.f);
		Time::targetFramerate(targetFrameRate);

		static float timeScale = 1.0;
		ImGui::SliderFloat("Timescale", &timeScale, 0.f, 10.f);
		Time::timeScale(timeScale);

		static float fixedFPS = 50.f;
		ImGui::SliderFloat("Fixed FPS", &fixedFPS, 10.f, 244.f);
		Time::fixedDeltaTime(1 / fixedFPS);

		ImGui::Checkbox("Gizmos", &graphics::Gizmos::enabled);

		ImGui::BeginDisabled(!graphics::Gizmos::enabled);

		ImGui::Checkbox("Collider", &graphics::Gizmos::drawCollider);
		ImGui::Checkbox("AABB", &graphics::Gizmos::aabb);

		ImGui::EndDisabled();

		ImGui::SliderFloat("Gizmos width", &graphics::Gizmos::lineWidth, 0.1f, 50.f);


		ImGui::ColorEdit4("Collider Color", &colliderColor[0]);
		ImGui::ColorEdit4("AABB Color", &aabbColor[0]);

		ImGui::End();
	}
}