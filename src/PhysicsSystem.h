#pragma once
#include "Core/ISystem.h"
#include "Core/Scene.h"
#include "Components/Rigidbody2D.h"
#include "Utils/Time.h"
#include <box2d/box2d.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace engine {
	class PhysicsSystem {
	public:
		void update();
	};
}