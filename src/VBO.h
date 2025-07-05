#pragma once

#include<glad/glad.h>
#include "glm/glm.hpp"
#include "array"
#include <cstdint> 

#include<vector>

namespace graphics {
	struct Vertex {
		uint16_t position[2];  // 2D position (X, Y)
		uint16_t texUV[2];     // 2D texture coordinates (U, V)
	};

	class VBO
	{
	public:
		GLuint ID;
		VBO(const std::array<graphics::Vertex, 4>& vertices);
		VBO() = default;

		// Binds the VBO
		void Bind();
		// Unbinds the VBO
		void Unbind();
		// Deletes the VBO
		void Delete();
	};
}