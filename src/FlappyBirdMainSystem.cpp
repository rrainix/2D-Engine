#include "FlappyBirdMainSystem.h"
#include "Utils/mathr.h"
#include "Utils/randomr.h"
#include "Utils/Async.h"

namespace engine {
	struct Deadly {};
	struct PlayerData {
		float movementSpeed = 5.f;
		float gravitySpeed = 10.f;
		float jumpForce = 6.f;
	};
	TextureHandle pipe;

	TextureHandle playerSprites[3];

	void FlappyBirdMainSystem::AddBorder(Scene& scene, glm::vec2 position, glm::vec2 scale) {
		entt::entity borderHandle = scene.createRenderableEntity(Transform2D::FromPositionScale(position, scale), SpriteRenderer::create(m_squareTex, 1, { 1,1,1, 0.2f }));
		scene.addComponent<BoxCollider>(borderHandle).registerContacts(true);
	}
	void FlappyBirdMainSystem::CreatePipe(Scene& scene) {
		float dst = rnd::next(6.f, 7.5f);
		float posY = rnd::next(-1.f, 7.f);

		glm::vec2 scale = TextureManager::getTexture(pipe).sizeNormalized()  *10.f;

		entt::entity borderHandle = scene.createRenderableEntity(Transform2D::FromPositionScaleRotation({ m_nextPipePositionX ,posY + dst }, scale, 180.f), SpriteRenderer::create(pipe, 1, { 1.f,1.f,1.f,1.f }));
		scene.addComponent<BoxCollider>(borderHandle).registerContacts(true);
		scene.addComponent<Deadly>(borderHandle);

		entt::entity borderHandle_2 = scene.createRenderableEntity(Transform2D::FromPositionScale({ m_nextPipePositionX ,posY - dst }, scale), SpriteRenderer::create(pipe, 1, { 1.f,1.f,1.f,1.f }));
		scene.addComponent<BoxCollider>(borderHandle_2).registerContacts(true);
		scene.addComponent<Deadly>(borderHandle_2);

		m_nextPipePositionX += rnd::next(5.f, 10.f);
	}

	void FlappyBirdMainSystem::update(Scene& scene) {
		if (Input::getKeyDown(KeyCode::r)) {
			SceneManager::reloadScene(scene.name());
		}

		bool getMouseDown = Input::getMouseButtonDown(0) || Input::getKeyDown(KeyCode::space);
		float size = m_camera->orthographicSize() + (-Input::scrollValue() * m_camera->orthographicSize() / 3.f);
		size = fmax<float>(size, 0.01f);
		m_camera->orthographicSize(size);

		auto playerEntity = scene.getSingletonEntity<PlayerData>();

		PlayerData& playerData = scene.getSingletonComponent<PlayerData>();

		float targetRot = math::clamp( m_playerRigidbody->getVelocity().y * 5, -75.f, 75.f);

		m_playerRigidbody->setVelocity({ playerData.movementSpeed,getMouseDown ? playerData.jumpForce : m_playerRigidbody->getVelocity().y });

		scene.getComponent<Transform2D>(playerEntity).setRotation(targetRot);

		if (m_nextPipePositionX - m_playerRigidbody->getPosition().x <= 100)
			CreatePipe(scene);
	}

	void FlappyBirdMainSystem::fixedUpdate(Scene& scene) {
		float smoothingSpeed = 5.0f;
		float dt = Time::fixedDeltaTime();
		float t = 1.0f - glm::exp(-smoothingSpeed * dt);
		glm::vec2 unclampedTarget = m_playerRigidbody->getPosition();
		glm::vec2 viewportHalfSize = m_camera->worldViewPort() / 2.f;
		// clamp camera center so viewport stays inside limits
		glm::vec2 target;
		target.x = unclampedTarget.x;
		target.y = std::clamp<float>(unclampedTarget.y, -5.f + viewportHalfSize.y, 15.f - viewportHalfSize.y);

		m_camera->transform->position = glm::mix(m_camera->transform->position, target, t);
	}

	void OnBeginContact(const b2ContactBeginTouchEvent& e) {
		Debug::log("Contact!");
		entt::entity handle_1 = Physics2D::fromShapeId(e.shapeIdA);
		entt::entity handle_2 = Physics2D::fromShapeId(e.shapeIdB);
		Scene& scene = SceneManager::getActiveScene();

		//if (scene.anyHasComponent<Deadly>(handle_1, handle_2))
		//	SceneManager::reloadScene(SceneManager::getActiveScene().name());
	}

	void switchSprites( entt::entity playerEntity) {
		Scene& scene = SceneManager::getActiveScene();

		while (true) {
			static int currentSprite = 0;
			scene.getComponent<SpriteRenderer>(playerEntity).texture = playerSprites[currentSprite];
			currentSprite = (currentSprite + 1 >= 3 ? 0 : currentSprite + 1);
			Async::waitForMilliseconds(100);
		}
	}

	void FlappyBirdMainSystem::start(Scene& scene) {
		m_camera = &scene.addCamera(scene.createTransformEntity(Transform2D::FromPosition(glm::vec2{ 0.f, 5.f })));
		m_camera->orthographicSize(6.f);

		auto texHandles = TextureManager::getLoadedHandles();

		pipe = texHandles[0];
		for (int i = 0; i < 3; i++) {
			playerSprites[i] = texHandles[1 + i];
		}

		AddBorder(scene, { 0, -5 }, { 1000.f, 0.1f });
		AddBorder(scene, { 0, 15 }, { 1000.f, 0.1f });

		// Player

		entt::entity playerEnt = scene.createRenderableEntity(Transform2D::FromPosition({ 0.f, 5.f }), SpriteRenderer::create(playerSprites[0], 0, {1,1,1,1}));
		Transform2D& tr = scene.getComponent<Transform2D>(playerEnt);
 
		ASYNC(switchSprites(playerEnt));

		m_playerRigidbody = &scene.addComponent<Rigidbody2D>(playerEnt);
		m_playerRigidbody->setGravityScale(3.f);
		CircleCollider& collider = scene.addComponent<CircleCollider>(playerEnt);
		scene.addComponent<PlayerData>(playerEnt);
		collider.setFriction(0);
		collider.radius(0.25f);
		collider.freezeRotation(true);


		collider.registerContacts(true);
		collider.onCollisionEnter(scene, OnBeginContact);

		tr.scale = TextureManager::getTexture(playerSprites[0]).sizeNormalized();
	}
}