#pragma once

#ifdef USE_BOX2D
#include <box2d/box2d.h>
namespace engine {
    struct BodyHandle { 
        b2BodyId id; 
        void Destroy() {
        if (b2Body_IsValid(id)) {
            b2DestroyBody(id);
        }
    } };
    struct ShapeHandle { b2ShapeId id; };
}
#elif defined(USE_CHIPMUNK)
#include <chipmunk/chipmunk.h>
namespace LLB {
    struct BodyHandle {
        cpBody* ptr;
        void Destroy() {
            cpBodyDestroy(ptr);
        }
    };
    struct ShapeHandle {
        cpShape* ptr; 
        void Destroy() {
            cpShapeDestroy(ptr);
        }
    };
}
#else
#error "Kein Physics-Backend definiert"
#endif