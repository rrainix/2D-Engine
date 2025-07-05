#pragma once

#include <Core/ISystem.h>
#include <glm/glm.hpp>
#include "Graphics/Camera.h"
#include "Utils/Input.h"
#include "Core/Window.h"
#include "Core/Scene.h"
#include "Components/Transform.h"
#include "Utils/Time.h"

namespace engine {
	class CameraSystem : public ISystem {
	public:
		void start(Scene& scene)override;
		void update(Scene& scene) override;

	private:
		graphics::Camera* m_camera;
		float m_lerpSize;
		float size = 15.0f;
	};
}