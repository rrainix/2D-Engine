
#ifdef USE_CHIPMUNK

#include "Physics/ChipmunkWorld.h"
#include <iostream>

namespace LLB {
	ChipmunkWorld::ChipmunkWorld() {
		space = cpSpaceNew();
		cpSpaceSetGravity(space, cpv(0, -9.8f));
	}

	ChipmunkWorld:: ~ChipmunkWorld() {
		cpSpaceFree(space);
	}

	void ChipmunkWorld::Step(float dt) {
		static bool t = false;
		if (!t) {
			cpSpaceSetIterations(space, 5);
			t = true;
		}

		cpSpaceStep(space, dt);
	}

	BodyHandle ChipmunkWorld::CreateBody(entt::entity handle, Scene& scene, BodyType bodyType) {
		Transform2D& tr = scene.GetComponent<Transform2D>(handle);

		cpBody* body = nullptr;

		if (bodyType == BodyType::Dynamic) {
			float moment = cpMomentForBox(1.f, tr.scale.x, tr.scale.y);
			body = cpBodyNew(1.0f, moment); // Mass = 1.0f, Moment = unendlich (kein Drehen)
		}
		else if (bodyType == BodyType::Static) {
			body = cpBodyNewStatic();
		}
		else if (bodyType == BodyType::Kinematic) {
			body = cpBodyNewKinematic();
		}

		cpBodySetPosition(body, cpv(tr.position.x, tr.position.y));
		cpBodySetAngle(body, tr.rotation);

		// Entity als UserData speichern (wichtig für spätere Collision-Callbacks)
		cpBodySetUserData(body, reinterpret_cast<cpDataPointer>(static_cast<uintptr_t>(handle)));

		cpSpaceAddBody(space, body);

		BodyHandle handleOut;
		handleOut.ptr = body;
		return handleOut;
	}

	void     ChipmunkWorld::SetLinearVelocity(BodyHandle h, const glm::vec2& v) { cpBodySetVelocity(h.ptr, cpVect(v.x, v.y)); }
	glm::vec2 ChipmunkWorld::GetPosition(BodyHandle h) { cpVect pos = cpBodyGetPosition(h.ptr); return { pos.x, pos.y }; }

	void ChipmunkWorld::SetGravityScale(float scale, BodyHandle h) {
		 
	}

	void ChipmunkWorld::SetPosition(const glm::vec2& position, BodyHandle h) { cpBodySetPosition(h.ptr, cpVect(position.x, position.y)); }
	void ChipmunkWorld::SetRotation(const float& radiant, BodyHandle h) { cpBodySetAngle(h.ptr, radiant); }

	ShapeHandle ChipmunkWorld::CreateShape(entt::entity handle, Scene& scene, BodyHandle bodyHandle, ShapeType shapeType) {
		if (shapeType == ShapeType::Box) {
			Transform2D transform = scene.GetComponent<Transform2D>(handle);
			cpShape* shape = cpBoxShapeNew(bodyHandle.ptr, transform.scale.x, transform.scale.y, 0.f);
			ShapeHandle shapeHandle;
			shapeHandle.ptr = shape;

			cpBodySetPosition(bodyHandle.ptr, cpv(transform.position.x, transform.position.y));
			cpBodySetAngle(bodyHandle.ptr, transform.rotation);

			// Shape in den Space einfügen
			cpShapeSetFriction(shape, 0.7f);    // normale Reibung
			cpShapeSetElasticity(shape, 0.0f);  // keine Bounciness
			cpSpaceAddShape(space, shape);

			return shapeHandle;
		}
		else if (shapeType == ShapeType::Circle)
		{

		}
	}
}

#endif