#include "Scene.h"

namespace engine {
	Box2DWorld Scene::s_physicsWorld = {};

	Scene::Scene(const std::string& name) : k_sceneName(name) {}

	Scene::~Scene() = default;

	entt::entity Scene::createEntity() { return	 m_registry.create(); }



	entt::entity Scene::createTransformEntity(const Transform2D& transform) {
		entt::entity entity = m_registry.create();
		addComponent<Transform2D>(entity, transform);
		return entity;
	}

	/// Creates a new entity with a transform and spriteRenderer component
	entt::entity Scene::createRenderableEntity(const Transform2D& transform, const graphics::SpriteRenderer& spriteRenderer) {
		entt::entity entity = m_registry.create();
		addComponent<Transform2D>(entity, transform);
		addComponent<graphics::SpriteRenderer>(entity, spriteRenderer);
		return entity;
	}

	/// Creates a new entity with a transform and camera component and sets it as main camera
	graphics::Camera& Scene::addCamera(entt::entity handle) {
		Window& activeWindow = Window::activeWindow();
		Transform2D* transform;

		if (hasComponent<Transform2D>(handle)) {
			transform = &this->getComponent<Transform2D>(handle);
		}
		else {
			Debug::logWarning("Entity handle has no Transform2D Component! Added component automatically.");
			transform = &addComponent<Transform2D>(handle);
		}

		graphics::Camera& camera = this->addComponent<graphics::Camera>(handle, graphics::Camera(activeWindow.size().x, activeWindow.size().y, transform));
		camera.setMain();
		return camera;
	}

	void Scene::destroyEntity(entt::entity handle) { m_registry.destroy(handle); }

	void Scene::awakeSystems() {
		for (auto& s : m_systems)
		{
			try {
				s->awake(*this);
			}
			catch (const std::runtime_error& e) {
				Debug::logError(e.what());
			}
		}
	}

	void Scene::startSystems() {
		for (auto& s : m_systems) {
			try {
				s->start(*this);
			}
			catch (const std::runtime_error& e) {
				Debug::logError(e.what());
			}
		}
	}

	void Scene::updateSystems() {
		for (auto& s : m_systems)
		{
			if (s->m_enabled)
			{
				try {
					s->update(*this);
				}
				catch (std::runtime_error e) {
					Debug::logError(std::string(e.what()));
				}
			}
		}
	}

	void Scene::fixedUpdateSystems() {
		for (auto& s : m_systems)
		{
			if (s->m_enabled) {
				try {
					s->fixedUpdate(*this);
				}
				catch (const std::runtime_error& e) {
					Debug::logError(e.what());
				}
			}
		}
	}

	void Scene::destroySystems() {
		for (auto& s : m_systems)
		{
			try
			{
				s->destroy(*this);
			}
			catch (const std::runtime_error& e) {
				Debug::logError(e.what());
			}
		}
	}

	Box2DWorld& Scene::physicsWorld() { return s_physicsWorld; }

	//entity handling

	bool Scene::isValid(entt::entity handle) const {
		return m_registry.valid(handle);
	}

	entt::registry& Scene::registry() { return m_registry; }

	const std::string& Scene::name() const { return k_sceneName; }

	bool Scene::isLoaded() const { return m_loaded; }

}