#include "RenderSystem.h"
#include "Graphics/TextureManager.h"
#include <execution>
#include <unordered_map>
#include "Core/Profiler.h"
#include "Utils/Debug.h"
#include "Core/DebugSettings.h"
#include <map>
#include "Utils/Tilemap.h"

namespace graphics {
	RenderSystem::RenderSystem() {
		glGenBuffers(1, &m_instanceColorVBO);
	}

	void RenderSystem::destroy() {
		ShaderManager::clear();
		glDeleteBuffers(1, &m_instanceColorVBO);
	}

	void RenderSystem::loadRenderSettings() {
		glClearColor(0.05f, 0.15f, 0.45f, 0.45f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		

		TextureManager::loadDefaultTextures();
		m_defaultShader = ShaderManager::loadShader("instanced.vert", "instanced.frag");
		m_debugShader = ShaderManager::loadShader("debug.vert", "debug.frag");
		m_tilemapShader = ShaderManager::loadShader("tilemap.vert", "tilemap.frag");
	}

	void RenderSystem::init() {
		loadRenderSettings();
		DebugRenderer::Init();
	}

	void RenderSystem::update() {
		Camera* camera = Camera::m_mainCamera;

		if (camera == nullptr) {
			float now = engine::Time::elapsedTime();
			static float lastDebugTime = now;

			if (now - lastDebugTime >= 1.f)
			{
				lastDebugTime = now;
				engine::Debug::logWarning("There is no active camera!");
			}
		}
		else {
			try {
				for (auto& scene : engine::SceneManager::loadedScenes)
				{
					render(*scene.get(), *camera);
					renderTilemaps(*scene.get(), *camera);
				}
				debugRender(*camera);
			}
			catch (std::runtime_error e) {
				engine::Debug::logError(e.what());
			}
		}


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		engine::Profiler::Get().Render();
		engine::DebugWindow::draw();
		engine::DebugSettings::Get().draw();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void RenderSystem::render(engine::Scene& scene, Camera& camera) {
		auto& registry = scene.registry();

		camera.updateViewYXZ();

		auto view = registry.view<engine::Transform2D, SpriteRenderer>();

		if (view.size_hint() == 0) {
			SET_GPU_STAT("Batches", std::to_string(0));
			SET_GPU_STAT("Triangles", std::to_string(0));
			SET_GPU_STAT("Vertices", std::to_string(0));
			return;
		}

		AABB camAABB = camera.viewportAABB();
		Gizmos::camViewportAABB = camAABB;

		struct SpriteInstance {
			glm::mat3 model;
			glm::vec4 color;
			TextureHandle texture;
			short layer;
		};

		// 1) Sichtbare Instanzen sammeln
		std::vector<SpriteInstance> instances;
		instances.reserve(view.size_hint());
		int renderObjects = 0;
		for (auto [ent, transform, sprite] : view.each()) {
			if (sprite.color.w <= 0.0f) continue;
			if (!AABB::intersects(AABB::create(transform), camAABB)) continue;

			instances.push_back({ transform.mat3(), sprite.color, sprite.texture, sprite.layer });
			++renderObjects;
		}


		if (renderObjects == 0) {
			SET_GPU_STAT("Batches", std::to_string(0));
			SET_GPU_STAT("Triangles", std::to_string(0));
			SET_GPU_STAT("Vertices", std::to_string(0));
			return;
		}

		// 2) Buckets erstellen: layer -> texture -> Liste von Instanzen

		std::unordered_map<short, std::unordered_map<TextureHandle, std::vector<SpriteInstance>>> buckets;
		// Dann passt reserve:
		buckets.reserve(16);
		for (auto& inst : instances) {
			buckets[inst.layer][inst.texture].push_back(inst);
		}

		// 3) Layers sortieren
		std::vector<short> layers;
		layers.reserve(buckets.size());
		for (auto& kv : buckets) layers.push_back(kv.first);
		std::sort(layers.begin(), layers.end());

		// 4) Shader & Uniforms
		Shader shader = ShaderManager::getShader(m_defaultShader);
		shader.Activate();
		shader.SetUniform("view", camera.viewMatrix());
		shader.SetUniform("projection", camera.projectionMatrix());
		GLint locTex = glGetUniformLocation(shader.ID, "texSampler");
		glUniform1i(locTex, 0);

		// 5) VAO nur einmal binden
		glBindVertexArray(m_spriteMesh.VAO.ID);

		uint16_t batches = 0;

		float start = engine::Time::elapsedTime();
		// 6) Durch alle Layers und Texturen iterieren
		for (short layer : layers) {
			auto& texMap = buckets[layer];
			for (auto& [texture, vec] : texMap) {
				size_t count = vec.size();

				// Modelle und Farben vorbereiten
				std::vector<glm::mat3> models;
				std::vector<glm::vec4> colors;
				models.reserve(count);
				colors.reserve(count);
				for (auto& inst : vec) {
					models.push_back(inst.model);
					colors.push_back(inst.color);
				}

				// Instanz-Daten hochladen
				glBindBuffer(GL_ARRAY_BUFFER, m_spriteMesh.instanceVBO);
				glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::mat3), models.data(), GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, m_instanceColorVBO);
				glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), colors.data(), GL_DYNAMIC_DRAW);
				// Color-Attrib für Instanzen konfigurieren (falls nicht bereits im VAO-Setup erfolgt)
				GLint colorLoc = 7; // entsprechen VAO-Attribut
				glEnableVertexAttribArray(colorLoc);
				glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
				glVertexAttribDivisor(colorLoc, 1);

				// Textur binden
				glActiveTexture(GL_TEXTURE0);
				TextureManager::getTexture(texture).Bind();

				// Draw Call
				glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(count));
				++batches;
			}
		}

		SET_GPU_STAT("Render", std::to_string(engine::Time::elapsedTime() - start) + "ms");
		SET_GPU_STAT("Triangles", std::to_string(0));
		SET_GPU_STAT("Batches", std::to_string(batches));
		SET_GPU_STAT("Triangles", std::to_string(renderObjects * 2));
		SET_GPU_STAT("Vertices", std::to_string(renderObjects * 4));

		// 8) Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void RenderSystem::renderTilemaps(engine::Scene& scene, Camera& camera)
	{
		auto& registry = scene.registry();

		Shader shader = ShaderManager::getShader(m_tilemapShader);
		shader.Activate();
	}

	void RenderSystem::debugRender(Camera& camera) {
		Shader debugShader = ShaderManager::getShader(m_debugShader);
		AABB camAABB = camera.viewportAABB();

		DebugRenderer::begin(debugShader, camera);
		int count = 0;

		// Draw all boxes
		for (auto& b : Gizmos::s_boxes) {
			DebugRenderer::drawBox(b.center, b.halfExtents, b.radiant, b.color);
			count++;
		}
		// Draw all lines
		for (auto& l : Gizmos::s_lines) {
			DebugRenderer::drawLine(l.start, l.end, l.color);
			count++;
		}
		// Draw all circles
		for (auto& c : Gizmos::s_circles) {
			DebugRenderer::drawCircle(c.center, c.radius, c.color, c.segments);
			count++;
		}

		DebugRenderer::render();
		Gizmos::clear();
	}
}