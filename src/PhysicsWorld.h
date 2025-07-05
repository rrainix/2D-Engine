//#pragma once
//
//#include <glm/vec2.hpp>
//#include <entt/entt.hpp>
//#include "BodyHandle.h"
//
//namespace engine {
//	class Scene;
//	class IPhysicsWorld {
//	public:
//		virtual ~IPhysicsWorld() = default;
//		virtual void Step(float dt) = 0;
//
//		//Rigidbody
//		virtual BodyHandle createBody(entt::entity handle, Scene& scene, BodyType bodyType) = 0;
//		virtual void        SetLinearVelocity(BodyHandle h, const glm::vec2& v) {}
//		virtual void        SetGravityScale(float scale, BodyHandle h) {}
//		virtual void SetPosition(const glm::vec2& position, BodyHandle h) {}
//		virtual void  SetRotation(const float& radiant, BodyHandle h) {}
//		virtual glm::vec2   GetPosition(BodyHandle h) { return glm::vec2{ 0.f }; }
//
//		//Collider
//		virtual ShapeHandle CreateShape(entt::entity handle, Scene& scene, BodyHandle bodyHandle, ShapeType shapeType) = 0;
//	};
//}