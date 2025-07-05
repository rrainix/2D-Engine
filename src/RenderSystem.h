#pragma once
#include "Camera.h"
#include <vector>
#include "SpriteMesh.h"
#include "Utils/Tilemap.h"
#include <random>
#include "Utils/Time.h"
#include <iostream>
#include "Graphics/Texture.h"
#include "Components/Transform.h"
#include "Core/Scene.h"
#include "Core/SceneManager.h"
#include "Components/Spriterenderer.h"
#include "Core/ISystem.h"
#include "chrono"
#include "Graphics/Gizmos.h"
#include "DebugRenderer.h"
#include "Utils/AABB.h"
#include "ShaderManager.h"
#include "Core/DebugWindow.h"

namespace graphics {
	class RenderSystem {
	public:
		void update();
		void init();
		void destroy();
		
		RenderSystem();
		~RenderSystem() = default;
		
	private:
		void loadRenderSettings();
		void render(engine::Scene& scene, Camera& camera);
		void renderTilemaps(engine::Scene& scene, Camera& camera);
		void debugRender(Camera& camera);

		ShaderId m_defaultShader;
		ShaderId m_debugShader;
		ShaderId m_tilemapShader;

		GLuint m_instanceModelVBO;
		GLuint m_instanceColorVBO;

		SpriteMesh m_spriteMesh;
	};
}