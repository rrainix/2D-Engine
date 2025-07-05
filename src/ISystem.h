#pragma once

#include "entt/entt.hpp"

namespace engine {
    class Scene;  // Forward declaration
}

namespace engine {
    class ISystem {
    public:
        virtual void update(Scene& scene) {}
        virtual void fixedUpdate(Scene& scene) {}
        virtual void awake(Scene& scene) {}
        virtual void start(Scene& scene) {}
        virtual void disable(Scene& scene) {}
        virtual void destroy(Scene& scene) {}
        bool enabled() { return m_enabled; }
        virtual ~ISystem() = default;

    private:
        bool m_enabled = true;

        friend class Scene;
    };
}