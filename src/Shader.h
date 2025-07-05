#pragma once

#include<glad/glad.h>
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include "Utils/Debug.h"

namespace graphics {
	class Shader
	{
	public:
		const ::std::string RESOURCE_SHADER_PATH = "Resources/Shader/";
		GLuint ID;
		Shader(const char* vertexFile, const char* fragmentFile);
		Shader() = default;

		void Activate();
		void Delete();

		void SetUniform(const ::std::string& name, int value) const;
		void SetUniform(const ::std::string& name, float value) const;
		void SetUniform(const std::string& name, const glm::vec2& value) const;
		void SetUniform(const ::std::string& name, const glm::vec3& value) const;
		void SetUniform(const ::std::string& name, const glm::vec4& value) const;
		void SetUniform(const ::std::string& name, const glm::mat4& value) const;
		void SetUniform(const ::std::string& name, const glm::mat3& value) const;
	private:
		::std::string GetFileContents(const char* filename);
		void CompileErrors(unsigned int shader, const char* type);
	};
}

