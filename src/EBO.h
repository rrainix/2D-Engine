#pragma once

#include<glad/glad.h>
#include<vector>
#include<array>

namespace graphics {
	class EBO
	{
	public:
		// ID reference of Elements Buffer Object
		GLuint ID;
		// Constructor that generates a Elements Buffer Object and links it to indices
		EBO(const std::array<GLuint,6>& indices);
		EBO() = default;

		// Binds the EBO
		void Bind();
		// Unbinds the EBO
		void Unbind();
		// Deletes the EBO
		void Delete();
	};
}
