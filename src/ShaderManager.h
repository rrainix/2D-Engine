#pragma once
#include "Shader.h"
#include <vector>


namespace graphics {
	struct ShaderId {
		uint8_t id;
	};

	class ShaderManager {
	public:
		static ShaderId loadShader(const std::string& vert, const std::string& frag);
		static Shader getShader(const ShaderId& hanlde);
		static void clear();

	private:
		static std::vector<Shader> m_shaders;
	};
}