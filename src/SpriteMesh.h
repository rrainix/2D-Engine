#pragma once

#include<string>

#include"VAO.h"
#include "Graphics/EBO.h"
#include "Camera.h"
#include <array>
#include"Texture.h"
#include <glm/packing.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/glm.hpp>

namespace graphics {
	class Camera;
}

namespace graphics {
	class SpriteMesh {
	public:
		const std::array<graphics::Vertex, 4> vertices = { {
		{{glm::packHalf1x16(0.5f),  glm::packHalf1x16(-0.5f)}, {glm::packHalf1x16(1.0f),  glm::packHalf1x16(0.0f)}},
		{{glm::packHalf1x16(-0.5f), glm::packHalf1x16(-0.5f)}, {glm::packHalf1x16(0.0f),  glm::packHalf1x16(0.0f)}},
		{{glm::packHalf1x16(-0.5f), glm::packHalf1x16(0.5f)}, {glm::packHalf1x16(0.0f),  glm::packHalf1x16(1.0f)}},
		{{glm::packHalf1x16(0.5f),  glm::packHalf1x16(0.5f)}, {glm::packHalf1x16(1.0f),  glm::packHalf1x16(1.0f)}}
	} };

		const std::array<GLuint, 6> indices = { 0, 2, 1, 0, 3, 2 };

		VAO VAO;
		VBO VBO;  // Member variable
		EBO EBO;  // Member variable

		SpriteMesh();
		~SpriteMesh();
		void Draw(Shader& shader, glm::mat3 modelMat3);
		void DrawInstanced(const std::vector<glm::mat3>& modelMatrices);
		GLuint instanceVBO = 0;
	private:

	};
}