#pragma once

// Definiere in deinen Project-Settings: USE_BOX2D oder USE_CHIPMUNK
#if defined(USE_BOX2D) && defined(USE_CHIPMUNK)
#error "Wähle genau ein Physics-Backend"
#elif !defined(USE_BOX2D) && !defined(USE_CHIPMUNK)
#error "Definiere USE_BOX2D oder USE_CHIPMUNK"
#endif

#if defined(USE_BOX2D)
#include "Physics/Box2DWorld.h"
#elif defined(USE_CHIPMUNK)
#include "Physics/ChipmunkWorld.h"
#endif