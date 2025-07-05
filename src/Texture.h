#pragma once

#include<glad/glad.h>
#include<stb/stb_image.h>
#include "Shader.h"
#include <string>
#include <memory>

namespace graphics {
	class Texture
	{
	public:
		GLuint ID;
		GLuint unit;


		const std::string RESOURCE_TEXTURE_PATH = "Resources/Textures/";

		Texture(Texture&& o) noexcept
			: ID(o.ID)
			, unit(o.unit)
			, m_size(o.m_size)
			, m_path(std::move(o.m_path))
		{
			o.ID = 0;
			o.unit = 0;
		}

		// Copy-Operationen löschen
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture() = default;
		~Texture();

		Texture(const char* imageName, GLuint slot, GLenum format, GLenum pixelType);

		void load(const char* imageName, GLuint slot, GLenum format, GLenum pixelType, int filterMode);

		// Assigns a texture unit to a texture
		void texUnit(Shader& shader, const char* uniform, GLuint unit);
		// Binds a texture
		void Bind();
		// Unbinds a texture
		void Unbind();
		// Deletes a texture
		void Delete() const;

		static void BindArray(Shader& shader, Texture textures[]);

		float aspect() const { return m_size.x / m_size.y; }
		glm::vec2 sizeNormalized() const { float max = std::max(m_size.x, m_size.y); return { m_size.x / max ,m_size.y / max }; }
		glm::vec2 size() { return m_size; }
		std::string path() { return m_path; }

	private:
		std::string m_path;
		glm::vec2 m_size;
	};
}