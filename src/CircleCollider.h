#pragma once
#include "Collider.h"
#include <box2d/box2d.h>

namespace engine {
	class CircleCollider : public Collider {
	public:
		CircleCollider(entt::entity handle, Scene& scene) : Collider(handle, scene)
		{
			shapeId = scene.physicsWorld().createShape(handle, scene, m_bodyId, ShapeType::Circle);
		}

		void radius(float radius) {
			b2Circle circle = b2Shape_GetCircle(shapeId);
			circle.radius = radius;
			b2Shape_SetCircle(shapeId, &circle);
		}

		float radius() {
			b2Circle circle = b2Shape_GetCircle(shapeId);
			return circle.radius;
		}


		void center(glm::vec2 center) {
			b2Circle circle = b2Shape_GetCircle(shapeId);
			circle.center = b2Vec2(center.x, center.y);
			b2Shape_SetCircle(shapeId, &circle);
		}

		glm::vec2 center() {
			b2Circle circle = b2Shape_GetCircle(shapeId);
			return glm::vec2{ circle.center.x, circle.center.y };
		}

		void debugDraw() override {
			graphics::Gizmos::drawCircle(bodyPosition(), radius(), 64);
		}
	};
}