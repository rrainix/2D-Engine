#pragma once
#include <vector>;
#include "Shader.h"
#include "Camera.h"
#include "Gizmos.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

namespace graphics {
	class DebugRenderer {
	public:
		static void Init();
		static void updateMaxVertices();
		static void begin(Shader& debugShader,Camera& camera);
		static void drawBox(const glm::vec2& center, const glm::vec2& halfExtents, float radians, const glm::vec4& color);
		static void drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color);
		static void drawCircle(const glm::vec2& center, float radius, const glm::vec4& color, int segments = 32);
		static void render();
	private:
		struct DebugVertex { glm::vec2 pos; glm::vec4 color; };
		static unsigned int vao, vbo;
		static std::vector<DebugVertex> vertices;
		static std::vector<int> batchSizes;
	};
}