#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "Core/Scene.h"
#include "Core/SceneManager.h"
#include "Physics/Box2DWorld.h"
#include "Physics/PhysicsWorld.h"
#include "Transform.h"
#include <iostream>

namespace engine {
	class Scene;
}

namespace engine {
	class Rigidbody2D {
	public:
		Rigidbody2D() = default;

		Rigidbody2D(entt::entity handle, Scene& scene) {
			m_bodyId = scene.physicsWorld().createBody(handle, scene, BodyType::Dynamic);
		}

		~Rigidbody2D() {
			if (b2Body_IsValid(m_bodyId)) {
				b2DestroyBody(m_bodyId);
			}
		}

		void rotationZ(bool enabled) {
			b2Body_SetFixedRotation(m_bodyId, !enabled);
		}
		bool rotationZ() {
			return b2Body_IsFixedRotation(m_bodyId);
		}

		void setVelocity(const glm::vec2& velocity) { b2Body_SetLinearVelocity(m_bodyId, b2Vec2(velocity.x, velocity.y)); }
		const glm::vec2& getVelocity() const {
			b2Vec2 b2v = b2Body_GetLinearVelocity(m_bodyId);
			return glm::vec2{ b2v.x, b2v.y };
		}

		void setAngularVelocity(float velocity) { b2Body_SetAngularVelocity(m_bodyId, velocity); }
		const float& getAngularVelocity() const { return b2Body_GetAngularVelocity(m_bodyId); }

		void setGravityScale(float gravityScale) { b2Body_SetGravityScale(m_bodyId, gravityScale); }
		const float& getGravityScale() const { return b2Body_GetGravityScale(m_bodyId); }

		void setMass(float mass) {
			auto massData = b2Body_GetMassData(m_bodyId);
			massData.mass = mass;
			b2Body_SetMassData(m_bodyId, massData);
		}
		const float& getMass() const { return b2Body_GetMass(m_bodyId); }

		void setLinearDrag(float value) {
			b2Body_SetLinearDamping(m_bodyId, value);
		}
		void setAngularDrag(float value) {
			b2Body_SetAngularDamping(m_bodyId, value);
		}


		void setPosition(const glm::vec2& position) { b2Body_SetTransform(m_bodyId, b2Vec2(position.x, position.y), b2Body_GetRotation(m_bodyId)); }
		glm::vec2 getPosition() { b2Vec2 b2Pos = b2Body_GetPosition(m_bodyId); return { b2Pos.x, b2Pos.y }; }
		float getRotation() { return b2Rot_GetAngle(b2Body_GetRotation(m_bodyId)); }

		void enable(bool enabled) {
			if (enabled)
				b2Body_Enable(m_bodyId);
			else
				b2Body_Disable(m_bodyId);
		}

		friend class CircleCollider;
		friend class BoxCollider;
		friend class Collider;
		friend class PhysicsSystem;
		friend class Physics2D;

	private:
		b2BodyId m_bodyId;
	};
}