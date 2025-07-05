#include "GameSystem.h"
#include "Graphics/TextureManager.h"
#include "Utils/serializer.h"

namespace engine {
	std::vector<graphics::TextureHandle> texHandles;

	void GameSystem::AddPhysicsEntity(Scene& scene, glm::vec2 pos, glm::vec2 scale = { 1.0f, 1.0f }, bool box = true) {
		entt::entity entity = scene.createRenderableEntity(Transform2D::FromPositionScaleRotation(pos, scale, 0.f), graphics::SpriteRenderer());
		entities.push_back(entity);
		auto& sp = scene.getComponent<graphics::SpriteRenderer>(entity);
		auto& tr = scene.getComponent<Transform2D>(entity);

		if (hasRb) {
			Rigidbody2D& rb = scene.addComponent<Rigidbody2D>(entity);
		}

		static int count = 0;
		count++;
		if (box) {
			BoxCollider& boxCollider = scene.addComponent<BoxCollider>(entity, scene);
			sp.texture = texHandles[rndm::Next(0, texHandles.size() - 1)];
			tr.scale = tr.scale * graphics::TextureManager::getTexture(sp.texture).sizeNormalized();
			boxCollider.updateScale(scene);
		}
		else {
			CircleCollider& circleCollider = scene.addComponent<CircleCollider>(entity, scene);
			sp.texture;
		}
	}

	void GameSystem::update(Scene& scene)
	{
		auto& registry = scene.registry();

		graphics::Gizmos::color = glm::vec4{ 0.0f, 1.f, 0.f, 1.0f };


		auto& camera = *graphics::Camera::main();

		glm::vec2 worldMousePos = camera.screenToWorld(Input::mousePosition());
		graphics::Gizmos::color = { 1.f, 0.f, 0.f, 1.f };
		graphics::Gizmos::drawCircle(worldMousePos, camera.orthographicSize() / 10, 32);


		if (entities.size() > 0 && scene.isValid(entities.back()))
			graphics::Gizmos::drawLine(scene.getComponent<Transform2D>(entities.back()).position, worldMousePos);

		graphics::Gizmos::color = { 0.f, scene.name() == "World 1" ? 1.f : 0.f, scene.name() != "World 1" ? 1.f : 0.f, 1.f };

		if ((Input::getKey(KeyCode::e) && spam) || Input::getKeyDown(KeyCode::e)) {
			AddPhysicsEntity(scene, worldMousePos);
		}
		if (Input::getKeyDown(KeyCode::f7)) {
			graphics::Gizmos::enabled = !graphics::Gizmos::enabled;
		}
		if ((Input::getKey(KeyCode::t) && spam) || Input::getKeyDown(KeyCode::t)) {
			AddPhysicsEntity(scene, worldMousePos, glm::vec2{ 1.f }, false);
		}

		if (Input::getKeyDown(KeyCode::b)) {
			for (int i = 0; i < 50; i++)
				AddPhysicsEntity(scene, worldMousePos);
		}
		if (Input::getMouseButton(2)) {
			auto rbView = registry.view<Rigidbody2D, Transform2D>();

			for (auto [ent, rb, tr] : rbView.each()) {
				glm::vec2 dir = worldMousePos - tr.position;
				rb.setVelocity(dir);
			}
		}

		if (Input::getKeyDown(KeyCode::d) && entities.size() > 0) {
			scene.destroyEntity(entities.back());
			entities.pop_back();
		}
		if (Input::getKeyDown(KeyCode::c)) {
			int size = 100;

			for (int y = -size; y < size; y++) {
				for (int x = -size; x < size; x++) {
					AddPhysicsEntity(scene, { x + worldMousePos.x * 1.2f, y + worldMousePos.y * 1.2f });
				}
			}
		}

		if (Input::getKeyDown(KeyCode::r)) {
			SceneManager::reloadScene(scene.name());
		}

		if (Input::getKeyDown(KeyCode::f1)) {
			hasRb = !hasRb;
		}
		if (Input::getKeyDown(KeyCode::f2)) {
			freezeX = !freezeX;
		}
		if (Input::getKeyDown(KeyCode::f3)) {
			freezeY = !freezeY;
		}
		if (Input::getKeyDown(KeyCode::f5)) {
			spam = !spam;
		}
	}



	void GameSystem::start(Scene& scene)
	{
		texHandles = graphics::TextureManager::getLoadedHandles();
		hasRb = false;

		AddPhysicsEntity(scene, glm::vec2{ 0.f, 0.f }, { 100.f, 1.f });
		AddPhysicsEntity(scene, glm::vec2{ 0.f, 50.f }, { 100.f, 1.f });
		AddPhysicsEntity(scene, glm::vec2{ -50.f , 25.f }, glm::vec2{ 1.f, 50.25f });
		AddPhysicsEntity(scene, glm::vec2{ 50.f , 25.f }, glm::vec2{ 1.f, 50.25f });

		useGravity = true;

		AddPhysicsEntity(scene, glm::vec2{ 0.f, 5.f }, glm::vec2{ 1.f, 1.33333333333f });
		AddPhysicsEntity(scene, glm::vec2{ -1.f, 1.f });
		hasRb = true;
	}
}