#include "DebugRenderer.h"

namespace graphics {
	unsigned int DebugRenderer::vao = 0, DebugRenderer::vbo = 0;
	std::vector<DebugRenderer::DebugVertex> DebugRenderer::vertices = std::vector<DebugRenderer::DebugVertex>();
	std::vector<int> DebugRenderer::batchSizes = std::vector<int>();

	void DebugRenderer::Init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, Gizmos::maxVertices() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)(sizeof(glm::vec2)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void DebugRenderer::updateMaxVertices() {
		glBufferData(GL_ARRAY_BUFFER, Gizmos::maxVertices() * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
	}

	void DebugRenderer::begin(Shader& debugShader, Camera& camera) {
		debugShader.Activate();
		glLineWidth(Gizmos::lineWidth);
		debugShader.SetUniform("u_ViewProj", camera.viewProjection());
		vertices.clear(); batchSizes.clear();
	}

	void DebugRenderer::drawBox(const glm::vec2& center, const glm::vec2& halfExtents, float radians, const glm::vec4& color)
	{
		glm::vec2 local[4] = {
	{-halfExtents.x, -halfExtents.y},
	{ halfExtents.x, -halfExtents.y},
	{ halfExtents.x,  halfExtents.y},
	{-halfExtents.x,  halfExtents.y}
		};


		if (!AABB::isAxisAligned(radians)) {
			float c = std::cos(radians);
			float s = std::sin(radians);
			for (int i = 0; i < 4; ++i)
			{
				glm::vec2 p = local[i];
				glm::vec2 rotated = {
					p.x * c - p.y * s,
					p.x * s + p.y * c
				};
				glm::vec2 worldPos = center + rotated;
				vertices.push_back({ worldPos, color });
			}
		}
		else {
			for (int i = 0; i < 4; ++i)
			{
				glm::vec2 p = local[i];
				glm::vec2 worldPos = center + p;
				vertices.push_back({ worldPos, color });
			}
		}

		batchSizes.push_back(4);
	}

	void DebugRenderer::drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {
		vertices.push_back({ start, color });
		vertices.push_back({ end, color });
		batchSizes.push_back(2);
	}

	void DebugRenderer::drawCircle(const glm::vec2& center, float radius, const glm::vec4& color, int segments) {
		constexpr float twoPi = 2.0f * glm::two_pi<float>();
		for (int i = 0; i < segments; ++i) {
			float theta = (float)i / (float)segments * twoPi;
			glm::vec2 p = center + glm::vec2(glm::cos(theta), glm::sin(theta)) * radius;
			vertices.push_back({ p, color });
		}
		batchSizes.push_back(segments);
	}

	void DebugRenderer::render() {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(DebugVertex), vertices.data());
		glBindVertexArray(vao);
		int offset = 0;
		for (int count : batchSizes) {
			glDrawArrays(GL_LINE_LOOP, offset, count);
			offset += count;
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}