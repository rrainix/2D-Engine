#include "Experimental/CameraSystem.h"

namespace engine {
	void CameraSystem::start(Scene& scene) {
		m_camera = &scene.addCamera(scene.createTransformEntity(Transform2D()));
	}

	void CameraSystem::update(Scene& scene) {
		if (Input::getMouseButton(1))
			m_camera->transform->position += Input::mouseAxis() * m_camera->orthographicSize() / 250.f;

		float speed = 2.25f * engine::Time::deltaTime() * m_camera->orthographicSize();
		size += -Input::scrollValue() * m_camera->orthographicSize() / 3.f;
		size = fmax<float>(size, 0.01f);
		m_lerpSize = size;
		m_lerpSize = std::lerp(m_lerpSize, size, Time::unscaledDeltaTime() * 5.f);

		m_camera->orthographicSize(m_lerpSize);
	}
}