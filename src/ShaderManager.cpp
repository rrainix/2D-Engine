#include "ShaderManager.h"

namespace graphics {
	std::vector<Shader> ShaderManager::m_shaders = {};

	ShaderId ShaderManager::loadShader(const std::string& vert, const std::string& frag) {
		m_shaders.push_back(Shader(vert.c_str(), frag.c_str()));
		return { static_cast<uint8_t>(m_shaders.size() - 1) };
	}
	Shader ShaderManager::getShader(const ShaderId& hanlde) {
		return m_shaders[hanlde.id];
	}

	void ShaderManager::clear() {
		for (auto& shader : m_shaders) {
			shader.Delete();
		}

		m_shaders.clear();
	}
}