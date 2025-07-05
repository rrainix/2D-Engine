#include "SpriteMesh.h"

namespace graphics {
	SpriteMesh::SpriteMesh()
	{
		// Set up VAO, VBO, and EBO for the quad
		VAO.Bind();
		VBO = graphics::VBO{ vertices };
		EBO = graphics::EBO{ indices };

		VAO.LinkAttrib(VBO,
			/*location=*/0,
			/*size=*/2,
			GL_HALF_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, position));

		// **UV auf location 1** (NICHT 2!):
		VAO.LinkAttrib(VBO,
			/*location=*/1,
			/*size=*/2,
			GL_HALF_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, texUV));

		// 2) Instanced-Model-Matrix (je 3 floats) an Locations 3,4,5
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (int i = 0; i < 3; ++i) {
			GLuint loc = 3 + i;
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
				sizeof(glm::mat3),
				(void*)(sizeof(glm::vec3) * i));
			glVertexAttribDivisor(loc, 1);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		VAO.Unbind();
		VBO.Unbind();
		EBO.Unbind();
	}

	SpriteMesh::~SpriteMesh() {
		VAO.Delete();
		VBO.Delete();
		EBO.Delete();

		if (instanceVBO != 0)
			glDeleteBuffers(1, &instanceVBO);
	}

	void SpriteMesh::Draw(Shader& shader, glm::mat3 modelMatrix)
	{
		VAO.Bind();

		glUniformMatrix3fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	void SpriteMesh::DrawInstanced(const std::vector<glm::mat3>& modelMatrices) {
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat3), modelMatrices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		VAO.Bind();
		// Draw all instances in one call.
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(modelMatrices.size()));
		VAO.Unbind();
	}
}
