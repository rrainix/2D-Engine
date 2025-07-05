#include"Shader.h"

namespace graphics {
	std::string Shader::GetFileContents(const char* filename)
	{
		std::string fullPath = RESOURCE_SHADER_PATH + filename;
		std::ifstream in((fullPath), std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}

	Shader::Shader(const char* vertexFile, const char* fragmentFile)
	{
		// Read vertexFile and fragmentFile and store the strings
		std::string vertexCode = GetFileContents(vertexFile);
		std::string fragmentCode = GetFileContents(fragmentFile);

		// Convert the shader source strings into character arrays
		const char* vertexSource = vertexCode.c_str();
		const char* fragmentSource = fragmentCode.c_str();

		// Create Vertex Shader Object and get its reference
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		// Attach Vertex Shader source to the Vertex Shader Object
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		// Compile the Vertex Shader into machine code
		glCompileShader(vertexShader);
		// Checks if Shader compiled succesfully
		CompileErrors(vertexShader, "VERTEX");

		// Create Fragment Shader Object and get its reference
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		// Attach Fragment Shader source to the Fragment Shader Object
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		// Compile the Vertex Shader into machine code
		glCompileShader(fragmentShader);
		// Checks if Shader compiled succesfully
		CompileErrors(fragmentShader, "FRAGMENT");

		// Create Shader Program Object and get its reference
		ID = glCreateProgram();
		// Attach the Vertex and Fragment Shaders to the Shader Program
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		// Wrap-up/Link all the shaders together into the Shader Program
		glLinkProgram(ID);
		// Checks if Shaders linked succesfully
		CompileErrors(ID, "PROGRAM");

		// Delete the now useless Vertex and Fragment Shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void Shader::Activate()
	{
		glUseProgram(ID);
	}
	void Shader::Delete()
	{
		glDeleteProgram(ID);
	}
	void Shader::CompileErrors(unsigned int shader, const char* type)
	{
		// Stores status of compilation
		GLint hasCompiled;
		// Character array to store error message in
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				engine::Debug::logError("SHADER_COMPILATION_ERROR for:" + std::string(type) + "\n" + infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				engine::Debug::logError("SHADER_LINKING_ERROR for:" + std::string(type == 0 ? "" : type) + "\n" + infoLog);
			}
		}
	}

	void Shader::SetUniform(const std::string& name, int value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			throw std::runtime_error("Warning: Uniform '" + name + "' not found in shader.");
			return;
		}
		glUniform1i(location, value);
	}
	void Shader::SetUniform(const std::string& name, const glm::vec2& value) const {
		GLint loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) {
			throw std::runtime_error("Warning: Uniform '" + name + "' not found.");
		}
		glUniform2f(loc, value.x, value.y);
	}

	void Shader::SetUniform(const std::string& name, float value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << name << "' not found in shader.\n";
			return;
		}
		glUniform1f(location, value);
	}

	void Shader::SetUniform(const std::string& name, const glm::vec3& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << name << "' not found in shader.\n";
			return;
		}
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void Shader::SetUniform(const std::string& name, const glm::vec4& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << name << "' not found in shader.\n";
			return;
		}
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void Shader::SetUniform(const std::string& name, const glm::mat4& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << name << "' not found in shader.\n";
			return;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::SetUniform(const std::string& name, const glm::mat3& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cerr << "Warning: Uniform '" << name << "' not found in shader.\n";
			return;
		}
		// RICHTIG: mat3 mit glUniformMatrix3fv hochladen
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

