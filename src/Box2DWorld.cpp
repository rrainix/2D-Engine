#include "Physics/Box2DWorld.h"
#include "Core/Scene.h"
#include <iostream>

namespace engine {
	Box2DWorld::Box2DWorld() {
		b2WorldDef def = b2DefaultWorldDef();
		def.gravity = b2Vec2{ 0, -9.8f };
		m_worldId = b2CreateWorld(&def);
	}
	Box2DWorld::~Box2DWorld() {
		b2DestroyWorld(m_worldId);
	}
	void Box2DWorld::Step(float dt) {
		b2World_Step(m_worldId, dt, 5);
	}

	//Rigidbody
	b2BodyId Box2DWorld::createBody(entt::entity handle, Scene& scene, BodyType bodyType) {
		Transform2D& tr = scene.getComponent<Transform2D>(handle);

		b2Vec2 box2dPos(tr.position.x, tr.position.y);


		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = bodyType == BodyType::Dynamic ? b2_dynamicBody : (bodyType == BodyType::Static ? b2_staticBody : b2_kinematicBody);
		bodyDef.gravityScale = 1.0f; 
		bodyDef.position = box2dPos;
		bodyDef.isBullet = true;
		bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(handle));
		bodyDef.linearDamping = 0.1f;

		b2BodyId bodyId = b2CreateBody(m_worldId, &bodyDef);
		return bodyId;
	}

	b2ShapeId Box2DWorld::createShape(entt::entity handle, Scene& scene, b2BodyId bodyId, ShapeType shapeType) {

		Transform2D transform = scene.getComponent<Transform2D>(handle);
		b2ShapeId shapeId = b2_nullShapeId;

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = 1.f;
		shapeDef.material.friction = 0.3f;
		shapeDef.material.restitution = 0.f;

		if (shapeType == ShapeType::Box) {

			b2Polygon b2Polygon = b2MakeBox(0.5f * transform.scale.x, 0.5f * transform.scale.y);
			shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &b2Polygon);
		}
		else if (shapeType == ShapeType::Circle)
		{
			float r = 0.25f * (transform.scale.x + transform.scale.y);
			b2Circle circle = { b2Vec2{0,0}, r };
			shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
		}


		b2Body_SetTransform(bodyId, b2Vec2(transform.position.x, transform.position.y), transform.b2Rotation());
		return shapeId;
	}

	CollisionDispatcher& Box2DWorld::dispatcher() { return m_dispatcher; }
}

