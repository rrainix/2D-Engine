#pragma once
#include "Core/ISystem.h"
#include "Core/Scene.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/Transform.h"
#include "Graphics/Gizmos.h"
#include "Core/DebugSettings.h"

namespace graphics {
	class GizmosRenderSystem : public engine::ISystem {
	public:
		void update(engine::Scene& scene) override;
	};
}