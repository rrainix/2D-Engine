#pragma once
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include "Rigidbody2D.h"
#include <iostream>
#include "Graphics/Gizmos.h"

namespace engine {
	class Collider {
		friend class Physics2D;

	public:
		Collider() = default;  // jetzt existiert wieder ein parameterloser Ctor


		Collider(entt::entity handle, Scene& scene) : handle{ handle } {
			if (scene.hasComponent<Rigidbody2D>(handle)) {
				m_bodyId = scene.getComponent<Rigidbody2D>(handle).m_bodyId;
			}
			else {
				m_bodyId = scene.physicsWorld().createBody(handle, scene, BodyType::Static);
			}
		}

		~Collider() {
			if (b2Body_IsValid(m_bodyId))
				b2DestroyBody(m_bodyId);
		}

		bool isValid() { return b2Body_IsValid(m_bodyId) && b2Shape_IsValid(shapeId); }
		void setFriction(float friction) { b2Shape_SetFriction(shapeId, friction); }
		void setBounciness(float bounciness) { b2Shape_SetRestitution(shapeId, bounciness); }
		glm::vec2 bodyPosition() {
			b2Vec2 position = b2Body_GetPosition(m_bodyId);
			return { position.x, position.y };
		}
		float bodyRotationDegrees() {
			return glm::degrees<float>(b2Rot_GetAngle(b2Body_GetRotation(m_bodyId)));
		}

		virtual void debugDraw() {};

		void registerContacts(bool enabled) {
			b2Shape_EnableContactEvents(shapeId, enabled);
		}
		bool registerContacts() { return b2Shape_AreContactEventsEnabled(shapeId); }

		template<typename F>
		void onCollisionEnter(Scene& scene, F&& callback) {
			if (!registerContacts()) {
				Debug::logWarning(
					"[BoxCollider] Failed to register onCollisionEnter event: contact events are not enabled for this shape. "
					"Make sure to call collider.registerContacts(true); before registering event callbacks."
				);
			}

			scene.physicsWorld().dispatcher().registerBegin(shapeId, std::forward<F>(callback));
		}
		template<typename F>
		void onCollisionExit(Scene& scene, F&& callback) {
			if (!registerContacts()) {
				Debug::logWarning(
					"[BoxCollider] Failed to register onCollisionExit event: contact events are not enabled for this shape. "
					"Make sure to call collider.registerContacts(true); before registering event callbacks."
				);
			}

			scene.physicsWorld().dispatcher().registerEnd(shapeId, std::forward<F>(callback));
		}
		template<typename F>
		void onCollisionHit(Scene& scene, F&& callback) {
			if (!registerContacts()) {
				Debug::logWarning(
					"[BoxCollider] Failed to register onCollisionHit event: contact events are not enabled for this shape. "
					"Make sure to call collider.registerContacts(true); before registering event callbacks."
				);
			}

			scene.physicsWorld().dispatcher().registerHit(shapeId, std::forward<F>(callback));
		}

		void freezeRotation(bool enabled) {
			b2Shape_SetDensity(shapeId, enabled ? 1.f : 0.f, false);
		}

		float getRotation() {
			b2Rot rotation = b2Body_GetRotation(m_bodyId);
			return  b2Rot_GetAngle(rotation);
		}

	private:
		void SetRotation(float radiant) {
			b2Body_SetTransform(m_bodyId, b2Body_GetPosition(m_bodyId), b2Rot(std::cos(radiant), std::sin(radiant)));
		}
		void SetPositionRotation(glm::vec2 position, float radiant) {
			b2Body_SetTransform(m_bodyId, b2Vec2(position.x, position.y), b2Rot(std::cos(radiant), std::sin(radiant)));
		}
		void setPosition(const glm::vec2& position) {
			b2Body_SetTransform(m_bodyId, b2Vec2(position.x, position.y), b2Body_GetRotation(m_bodyId));
		}

	protected:
		b2BodyId m_bodyId;
		b2ShapeId shapeId;
	    
		entt::entity handle;
	};
}