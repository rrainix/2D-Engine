#include "SceneManager.h"
#include "Physics/PhysicsSystem.h"
#include "Graphics/RenderSystem.h"
#include "Experimental/GameSystem.h" 
#include "Experimental/CameraSystem.h" 
#include "Graphics/GizmosRenderSystem.h"
#include <iostream>
#include <thread>
#include "Utils/Debug.h"
#include "Core/Profiler.h"

namespace engine {
	std::vector<std::string> SceneManager::availableScenes = {};
	std::vector<std::unique_ptr<Scene>> SceneManager::loadedScenes = {};
	Scene* SceneManager::activeScene = nullptr;

	void SceneManager::createScene(const std::string& name) { availableScenes.push_back(name); }

	void callAfter(std::function<void()> fn, int delayMs) {
		std::thread([fn, delayMs]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
			fn();
			}).detach();
	}

	/// Loads a new scene
	Scene& SceneManager::loadScene(const std::string& name) {
		// 1) Existenz in availableScenes prüfen
		auto it = std::find(availableScenes.begin(), availableScenes.end(), name);
		if (it == availableScenes.end())
		{
			throw std::runtime_error("Scene with name " + name + " can't be found. Call SceneManager::CreateScene(" + name + ") before loading the scene");
		}

		// 2) Double‑load abfangen
		if (std::any_of(loadedScenes.begin(), loadedScenes.end(),
			[&](auto& up) { return up->name() == name; }))
		{
			throw std::runtime_error("Scene with name " + name + " is already loaded. When loading multiple scenes call SceneManager::LoadSceneAdditive(" + name + ")");
		}

		// 3) ID aus der Position ermitteln
		uint8_t id = static_cast<uint8_t>(std::distance(availableScenes.begin(), it));
		// (optional: availableScenes.erase(it);)

		// 4) Szene anlegen und Systeme registrieren
		auto sceneUptr = std::make_unique<Scene>(name);


		// 5) in den Vector einfügen und activeScene aktualisieren
		loadedScenes.emplace_back(std::move(sceneUptr));
		Scene& sceneRef = *loadedScenes.back();
		activeScene = &sceneRef;
		sceneRef.m_loaded = true;
		activeScene = &sceneRef;
		return sceneRef;
	}

	Scene& SceneManager::reloadScene(const std::string& name) {
		auto it = std::find_if(loadedScenes.begin(), loadedScenes.end(),
			[&](const std::unique_ptr<Scene>& ptr) { return ptr->name() == name; });

		if (it == loadedScenes.end()) {
			throw std::runtime_error("Scene with name " + name + " is not loaded.");
		}

		Scene& scene = **it;
		scene.m_systems.clear();
		scene.m_registry.clear();
		scene.instantiateSystemsFromFactories();


		scene.awakeSystems();
		scene.startSystems();

		return scene;
	}

	void SceneManager::unloadScene(const std::string& name) {
		if (loadedScenes.empty()) {
			throw std::runtime_error("No scene is loaded.");
		}

		auto it = std::find_if(loadedScenes.begin(), loadedScenes.end(),
			[&name](const std::unique_ptr<Scene>& scene) {
				return scene->name() == name;
			});

		if (it == loadedScenes.end()) {
			throw std::runtime_error("Scene '" + name + "' is not loaded.");
		}

		(*it)->m_loaded = false;
		(*it)->destroySystems();
		(*it)->m_registry.clear();

		loadedScenes.erase(it);

		if (activeScene->name() == name) {
			if (!loadedScenes.empty())
				activeScene = loadedScenes.back().get();
			else
				activeScene = nullptr;
		}
	}

	Scene& SceneManager::getLoadedScene(const std::string& name) {
		auto it = std::find_if(loadedScenes.begin(), loadedScenes.end(),
			[&name](const std::unique_ptr<Scene>& scene) {
				return scene->name() == name;
			});

		if (it == loadedScenes.end()) {
			throw std::runtime_error("Scene with name " + name + " is not loaded.");
		}
		return *it->get();
	}

	Scene& SceneManager::getActiveScene() {
		if (activeScene == nullptr)
			throw std::runtime_error("There is no active Scene loaded.");

		return *activeScene;
	}

	void SceneManager::updateScenes() {
		PROFILE_CPU("Update", Profiler::CPUCategory::Update);

		for (auto& scene : loadedScenes) {
			if (scene->isLoaded())
			{
				scene->updateSystems();
			}
		}
	}

	void SceneManager::fixedUpdateScenes() {
		PROFILE_CPU("Fixedupdate", Profiler::CPUCategory::Fixedupdate);

		for (auto& scene : loadedScenes) {
			if (scene->isLoaded())
			{
				scene->fixedUpdateSystems();
			}
		}
	}
}