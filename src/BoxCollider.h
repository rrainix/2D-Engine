#pragma once
#include "Collider.h"
#include <box2d/box2d.h>
#include "Rigidbody2D.h"

namespace engine {
	class BoxCollider : public Collider {
	public:
		BoxCollider() = default;

		BoxCollider(entt::entity handle, Scene& scene) : Collider(handle, scene)
		{
			shapeId = scene.physicsWorld().createShape(handle, scene, m_bodyId, ShapeType::Box);
		}

		void scale(glm::vec2 scale, Scene& scene) {
			glm::vec2 center = this->center();
			Transform2D tr = scene.getComponent<Transform2D>(handle);
			b2Polygon polygon = b2MakeOffsetBox(tr.scale.x * scale.x * 0.5f, tr.scale.y * scale.y * 0.5f, b2Vec2(center.x, center.y), tr.b2Rotation());
			b2Shape_SetPolygon(shapeId, &polygon);
		}

		glm::vec2 scale() {
			b2ShapeType shapeType = b2Shape_GetType(shapeId);

			if (shapeType == b2_polygonShape) {
				b2Polygon polygon = b2Shape_GetPolygon(shapeId);

				if (polygon.count == 4) {
					glm::vec2 size = glm::vec2(
						polygon.vertices[2].x - polygon.vertices[0].x,
						polygon.vertices[2].y - polygon.vertices[0].y
					);
					return size;
				}
				else {
					throw std::runtime_error("This boxshape polygon count equals " + std::to_string(polygon.count) + " instead of 4");
				}
			}
			else {
				throw std::runtime_error("This box shapetype isn't type of b2_polygonShape");
			}

			return glm::vec2(1.0f);
		}

		glm::vec2 localScale(Scene& scene) {
			Transform2D tr = scene.getComponent<Transform2D>(handle);

			b2ShapeType shapeType = b2Shape_GetType(shapeId);
			if (shapeType == b2_polygonShape) {
				b2Polygon polygon = b2Shape_GetPolygon(shapeId);

				if (polygon.count == 4) {
					glm::vec2 size = glm::vec2(
						polygon.vertices[2].x - polygon.vertices[0].x,
						polygon.vertices[2].y - polygon.vertices[0].y
					);
					size += glm::vec2{ 1.f };
					return size - tr.scale;
				}
				else {
					throw std::runtime_error("This boxshape polygon count equals " + std::to_string(polygon.count) + " instead of 4");
				}
			}
			else {
				throw std::runtime_error("This box shapetype isn't type of b2_polygonShape");
			}

			return glm::vec2(1.0f);
		}

		// has to be called when the transforms scale has been changed.
		void updateScale(Scene& scene) {
			glm::vec2 center = this->center();
			Transform2D tr = scene.getComponent<Transform2D>(handle);

			b2Polygon polygon = b2MakeOffsetBox(tr.scale.x * 0.5f, tr.scale.y * 0.5f, b2Vec2(center.x, center.y), tr.b2Rotation());
			b2Shape_SetPolygon(shapeId, &polygon);
		}

		void center(glm::vec2 center, Scene& scene) {
			Transform2D tr = scene.getComponent<Transform2D>(handle);
			b2Polygon polygon = b2MakeOffsetBox(tr.scale.x * 0.5f, tr.scale.y * 0.5f, b2Vec2(center.x, center.y), b2Rot_identity);
			b2Shape_SetPolygon(shapeId, &polygon);
		}
		glm::vec2 center() {
			b2Polygon polygon = b2Shape_GetPolygon(shapeId);
			return glm::vec2(polygon.centroid.x, polygon.centroid.y);
		}

		float rotation() {
			b2Rot rotation = b2Body_GetRotation(m_bodyId);
			return  b2Rot_GetAngle(rotation);
		}

		void debugDraw() override {
			graphics::Gizmos::drawBox(bodyPosition() + center(), scale(), bodyRotationDegrees());
		}

	private:
		void setTransform(glm::vec2 position, glm::vec2 scale, float radiant, Scene& scene) {
			b2Body_SetTransform(m_bodyId, b2Vec2(position.x, position.y), b2Rot(std::cos(radiant), std::sin(radiant)));
			this->scale(glm::vec2{ 1.f }, scene);
		}

		friend class PhysicsSystem;
	};
}