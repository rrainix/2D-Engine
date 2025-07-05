#include "Physics/PhysicsSystem.h"
#include "Components/BoxCollider.h"
#include <iostream>

namespace engine {
	void PhysicsSystem::update() {
		Box2DWorld& physicsWorld = Scene::physicsWorld();
		physicsWorld.Step(Time::fixedDeltaTime());
		physicsWorld.dispatcher().process(physicsWorld.worldID());

		for (auto& scene : SceneManager::loadedScenes)
			for (auto [ent, rb, tf] : scene->registry().view<Rigidbody2D, Transform2D>().each()) {
				glm::vec2 targetPos = rb.getPosition();
				tf.position = targetPos;
				tf.rotation = rb.getRotation();
			}
	}
}