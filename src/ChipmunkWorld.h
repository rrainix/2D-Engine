#pragma once
#ifdef USE_CHIPMUNK

#include "Physics/PhysicsWorld.h"
#include <chipmunk/chipmunk.h>
#include "Components/Transform.h"
#include "Core/Scene.h"

namespace LLB {
  class ChipmunkWorld : public IPhysicsWorld {
    public:
        ChipmunkWorld();
        ~ChipmunkWorld() override;
        void Step(float dt) override;

        //Rigidbody

        BodyHandle CreateBody(entt::entity handle, Scene& scene, BodyType bodyType) override;
        void        SetLinearVelocity(BodyHandle h, const glm::vec2& v) override;
        glm::vec2   GetPosition(BodyHandle h) override;
        void SetPosition(const glm::vec2& position, BodyHandle h) override;
        void SetRotation(const float& radiant, BodyHandle h) override;
        void SetGravityScale(float scale, BodyHandle h) override;

        //Shape
        ShapeHandle CreateShape(entt::entity handle, Scene& scene, BodyHandle bodyHandle, ShapeType shapeType) override;

    private:
        cpSpace* space;
    };

}

#endif