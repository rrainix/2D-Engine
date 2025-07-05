#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "ISystem.h"
#include "Components/Transform.h"
#include "Components/Spriterenderer.h"
#include "Physics/Box2DWorld.h"
#include "Graphics/Camera.h"
#include "Core/Window.h"

#include <entt/entt.hpp>
#include <tuple>
#include <type_traits>
#include <fstream>
#include <stdexcept>
#include "Utils/Debug.h"
#include "Physics/CollisionDispatcher.h"

namespace engine {
	class ISystem;
	class SceneManager;
	class Application;
}

namespace engine {
	class Scene {
	public:
		Scene(const std::string& name);
		~Scene();



#pragma region ENTITY
		entt::entity createEntity();

		template<typename TComponent, typename... Args>
		entt::entity createEntity(Args&&... args) {
			auto entity = m_registry.create();
			m_registry.emplace<TComponent>(entity, std::forward<Args>(args)...);
			return entity;
		}

		template<typename... TComponent>
		entt::entity createEntity() {
			auto entity = m_registry.create();
			(m_registry.emplace<TComponent>(entity), ...);
			return entity;
		}

		entt::entity createTransformEntity(const Transform2D& transform);
		entt::entity createRenderableEntity(const Transform2D& transform, const graphics::SpriteRenderer& spriteRenderer);
		graphics::Camera& addCamera(entt::entity handle);
		void destroyEntity(entt::entity handle);

		bool isValid(entt::entity handle) const;
#pragma endregion

#pragma region COMPONENT
		template<typename TComponent, typename... Args>
	    requires (!std::is_empty_v<TComponent>)
		TComponent& addComponent(entt::entity handle, Args&&... args) {
			if (m_registry.all_of<TComponent>(handle)) {
				throw std::runtime_error("Component of type \"" + std::string(typeid(TComponent).name()) + "\" already exists on entity.");
			}

		
			// T(entt::entity, Scene&, Args...)
			if constexpr (std::is_constructible_v<TComponent, entt::entity, Scene&, Args...>) {
				return m_registry.emplace<TComponent>(
					handle,
					handle,
					*this,
					std::forward<Args>(args)...
				);
			}
			// T(entt::entity, Args...)
			else if constexpr (std::is_constructible_v<TComponent, entt::entity, Args...>) {
				return m_registry.emplace<TComponent>(
					handle,
					handle,
					std::forward<Args>(args)...
				);
			}
			else if constexpr (sizeof...(Args) == 0 && std::is_default_constructible_v<TComponent>) {
				return m_registry.emplace<TComponent>(handle);
			}
			else {
				return m_registry.emplace<TComponent>(handle, std::forward<Args>(args)...);
			}
		}

		template<typename TComponent>
			requires std::is_empty_v<TComponent>
		void addComponent(entt::entity handle) {
			if (m_registry.all_of<TComponent>(handle))
				throw std::runtime_error("...");

			m_registry.emplace<TComponent>(handle);
		}

		template<typename TComponent>
		bool hasComponent(entt::entity handle) const {
			return m_registry.all_of<TComponent>(handle);
		}
		template<typename... TComponent>
		bool hasAnyComponent(entt::entity handle) const {
			return m_registry.any_of<TComponent...>(handle);
		}

		template<typename... TComponent, typename... TEntity>
		bool anyHasComponent(TEntity... handles) const {
			return (... || m_registry.any_of<TComponent...>(handles));
		}

		template<typename TComponent>
		TComponent& getComponent
		(entt::entity handle) {
			return m_registry.get<TComponent>(handle);
		}
		template<typename TComponent>
		void removeComponent(entt::entity handle) {
			m_registry.remove<TComponent>(handle);
		}


		template<typename TComponent>
		TComponent& getSingletonComponent() {
			auto view = m_registry.view<TComponent>();
			const auto count = view.size();
			if (count == 0) {
				throw std::runtime_error(
					"Component of type \"" +
					std::string(typeid(TComponent).name()) +
					"\" not found"
				);
			}
			if (count > 1) {
				throw std::runtime_error(
					"More than one component of type \"" +
					std::string(typeid(TComponent).name()) +
					"\" exists in the registry, found " +
					std::to_string(count)
				);
			}

			auto entity = *view.begin();
			return view.get<TComponent>(entity);
		}
		template<typename TComponent>
		entt::entity getSingletonEntity() {
			auto view = m_registry.view<TComponent>();

			const auto count = view.size();
			if (count == 0) {
				throw std::runtime_error(
					"Component of type \"" +
					std::string(typeid(TComponent).name()) +
					"\" not found"
				);
			}
			if (count > 1) {
				throw std::runtime_error(
					"More than one component of type \"" +
					std::string(typeid(TComponent).name()) +
					"\" exists in the registry, found " +
					std::to_string(count)
				);
			}

			return *view.begin();
		}
#pragma endregion

#pragma region SYSTEM
		template<typename T, typename... Args>
		T& addSystem(Args&&... args) {
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
			auto sys = std::make_unique<T>(std::forward<Args>(args)...);
			T& ref = *sys;
			ref.awake(*this);
			ref.start(*this);
			m_systems.push_back(std::move(sys));
			m_systemFactories.push_back([
				// Kopiere notwendige Argumente in den Lambda-Capture
				capt_args = std::tuple<Args...>(std::forward<Args>(args)...)
			]() mutable {
					return std::apply(
						[](auto&&... a) { return std::make_unique<T>(std::forward<decltype(a)>(a)...); },
						capt_args
					);
				});
				return ref;
		}

		template<typename T>
		T& getSystem() {
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

			for (auto& sysPtr : m_systems) {
				if (auto ptr = dynamic_cast<T*>(sysPtr.get())) {
					return *ptr;
				}
			}
		}
		template<typename T>
		void disableSystem() {
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

			for (auto& sysPtr : m_systems) {
				if (auto ptr = dynamic_cast<T*>(sysPtr.get())) {
					if (ptr->m_enabled) {
						ptr->disable(*this);
						ptr->m_enabled = false;
					}
				}
			}
		}
		template<typename T>
		void enableSystem() {
			static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

			for (auto& sysPtr : m_systems) {
				if (auto ptr = dynamic_cast<T*>(sysPtr.get())) {
					ptr->m_enabled = true;
				}
			}
		}
#pragma endregion





		// Physics
		static Box2DWorld& physicsWorld();

		// Info
		entt::registry& registry();
		const std::string& name() const;
		bool isLoaded() const;

	private:
		void awakeSystems();
		void startSystems();
		void updateSystems();
		void fixedUpdateSystems();
		void destroySystems();
		void instantiateSystemsFromFactories() {
			m_systems.clear();
			for (auto& factory : m_systemFactories) {
				m_systems.push_back(factory());
			}
		}

		entt::registry m_registry;
		static Box2DWorld s_physicsWorld;
		std::vector<std::function<std::unique_ptr<ISystem>()>> m_systemFactories;
		std::vector<std::unique_ptr<ISystem>> m_systems;
		const std::string k_sceneName;

		bool m_loaded = false;
		friend class SceneManager;
		friend class Application;
	};

} // namespace LLB