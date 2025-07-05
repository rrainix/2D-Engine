#pragma once

#include "Core/ISystem.h"
#include "Components/Transform.h"
#include"Components/Spriterenderer.h"
#include "LLB/Random.h"
#include "Utils/Physics2D.h"

#include "Core/Scene.h"

#include <glm/glm.hpp>
#include "Time.h"
#include "Utils/Input.h"
#include "Components/Rigidbody2D.h"
#include "Graphics/Camera.h"
#include <tuple>
#include "Components/CircleCollider.h"
#include "Components/BoxCollider.h"
#include "Graphics/Gizmos.h"

#include "Physics/PhysicsSystem.h"

#include <box2d/box2d.h>

namespace engine {
	class GameSystem : public ISystem  {
	public:
		void start(Scene& scene)override;
		void update(Scene& scene) override;
		void AddPhysicsEntity(Scene& scene, glm::vec2 pos, glm::vec2 scale, bool box);


	private:
		std::vector<entt::entity> entities;
		bool useGravity = true;
		bool freezeX, freezeY;
		bool spam = true;
		bool hasRb = true;
	};
}