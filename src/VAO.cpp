#include"VAO.h"

namespace graphics {
	// Constructor that generates a VAO ID
	VAO::VAO()
	{
		glGenVertexArrays(1, &ID);
	}

	// Links a VBO to the VAO using a certain layout
	void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLboolean normalized, GLsizeiptr stride, void* offset)
	{
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, normalized, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
	}

	// Binds the VAO
	void VAO::Bind()
	{
		glBindVertexArray(ID);
	}

	// Unbinds the VAO
	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	// Deletes the VAO
	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
}