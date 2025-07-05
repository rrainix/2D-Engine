#pragma once
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include "Physics/CollisionDispatcher.h"

namespace engine {
    class Scene;
}

namespace engine {
    enum  class ShapeType { Box, Circle, Polygon };
    enum class BodyType { Static, Kinematic, Dynamic };

    class Box2DWorld {
        friend class Physics2D;

    public:
        Box2DWorld();
        ~Box2DWorld();

        void Step(float dt);

        //Rigidbody
        b2BodyId createBody(entt::entity handle, Scene& scene, BodyType bodyType);
        //Shape
        b2ShapeId createShape(entt::entity handle, Scene& scene, b2BodyId bodyId, ShapeType shapeType);

        CollisionDispatcher& dispatcher();
        b2WorldId worldID() { return m_worldId; }

    private:
        b2WorldId m_worldId;
        CollisionDispatcher m_dispatcher{};
    };
}