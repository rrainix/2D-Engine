#pragma once
#include "Scene.h"
#include <string>
#include <memory>
#include <vector>

namespace engine {
    class Scene;

    class SceneManager {
    public:
        static void createScene(const std::string& name);
        static  Scene& loadScene(const std::string& name);
        static Scene& reloadScene(const std::string& name);
        static void unloadScene(const std::string& name);
        static Scene& getLoadedScene(const std::string& name);
        static Scene& getActiveScene();
        static void updateScenes();
        static void fixedUpdateScenes();

        static std::vector<std::unique_ptr<Scene>> loadedScenes;

    private:
        static std::vector<std::string> availableScenes;
        static Scene* activeScene;
    };
}