#include "GizmosRenderSystem.h"
#include "Utils/AABB.h"


namespace graphics {
	void GizmosRenderSystem::update(engine::Scene& scene) {
		if (!Gizmos::enabled)
			return;

		auto& registry = scene.registry();


		//float transition = 25;
		//float phase = glm::fract(tr.position.x / transition) * glm::two_pi<float>();


		//float r = 0.5f * (std::sin(phase) + 1.0f);
		//float g = 0.5f * (std::sin(phase + 2.0f / 3.0f * glm::two_pi<float>()) + 1.0f);
		//float b = 0.5f * (std::sin(phase + 4.0f / 3.0f * glm::two_pi<float>()) + 1.0f);

		Gizmos::color = engine::DebugSettings::Get().colliderColor;

		if (Gizmos::drawCollider) {
			for (auto [ent, boxCollider, tr] : registry.view<engine::BoxCollider, engine::Transform2D>().each()) {
				boxCollider.debugDraw();
			}
			for (auto [ent, circleCollider, tr] : registry.view<engine::CircleCollider, engine::Transform2D>().each()) {
				circleCollider.debugDraw();
			}
		}

		Gizmos::color = engine::DebugSettings::Get().aabbColor;

		if (Gizmos::aabb) {
			for (auto [ent, tr, spr] : registry.view<engine::Transform2D, SpriteRenderer>().each()) {
				Gizmos::drawBox(tr.position, AABB::create(tr).scale(), 0.f);
			}
		}
	}
}