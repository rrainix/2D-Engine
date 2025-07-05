#pragma once
#include "EngineMain.h"

namespace engine {
	class FlappyBirdMainSystem : public ISystem {
	public:
		void update(Scene& scene)override;
		void start(Scene& scene)override;
		void fixedUpdate(Scene& scene)override;
	private:
		void AddBorder(Scene& scene, glm::vec2 position, glm::vec2 scale);
		void CreatePipe(Scene& scene);
		TextureHandle m_capsuleTex = TextureManager::getDefaultTexture(DefaultTexture::Capsule);
		TextureHandle m_circleTex = TextureManager::getDefaultTexture(DefaultTexture::Circle);
		TextureHandle m_squareTex = TextureManager::getDefaultTexture(DefaultTexture::Square);
		TextureHandle m_diamondTex = TextureManager::getDefaultTexture(DefaultTexture::IsometricDiamond);

		Camera* m_camera;

		std::vector<glm::vec2> poses;

		Rigidbody2D* m_playerRigidbody;
		float m_nextPipePositionX = 10.f;
	};
}