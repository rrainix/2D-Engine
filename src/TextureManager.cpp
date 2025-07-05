#include "Graphics/TextureManager.h"
#include "Utils/Debug.h"

namespace graphics {
	std::vector<TextureEntry> TextureManager::m_textures = {};
	std::queue<uint16_t> TextureManager::freeIndices = {};
    bool TextureManager::m_defaultTexLoaded = false;
    std::string TextureManager::m_defaultTextures[7] = 
    { 
        "Circle.png",
        "Square.png", 
        "Capsule.png", 
        "IsometricDiamond.png",
        "HexagonFlatTop.png",
        "HexagonPointedTop.png",
        "9sliced.png"
    };

    TextureHandle TextureManager::loadTexture(const std::string& name, FilterMode filterMode) {
        if (!m_defaultTexLoaded)
        {
            engine::Debug::logError("Call loadTexture(" + name + ") after creating the Application.");
            return {};
        }

        uint16_t index;
        if (!freeIndices.empty()) {
            // recycle
           
            index = freeIndices.front();
            freeIndices.pop();
            auto& entry = m_textures[index];

            entry.texture.Delete();                  // alten Handle freigeben
            entry.texture.load(name.c_str(),        // direkt re-load ins Entry
                index,
                GL_RGBA,
                GL_UNSIGNED_BYTE, static_cast<int>(filterMode));
            entry.generation++;
            entry.valid = true;
            entry.name = name;
        }
        else {
            index = static_cast<uint16_t>(m_textures.size());
            TextureEntry entry;
            entry.texture.load(name.c_str(),        // neu laden
                index,
                GL_RGBA,
                GL_UNSIGNED_BYTE, static_cast<int>(filterMode));
            entry.generation = 0;
            entry.valid = true;
            entry.name = name;
            m_textures.push_back(std::move(entry));
        }

        return { index, m_textures[index].generation };
    }

    TextureHandle TextureManager::getDefaultTexture(DefaultTexture type) {
        int index = static_cast<int>(type);

        if (index < 0 || index >= std::size(m_defaultTextures)) {
            throw std::runtime_error("Invalid DefaultTexture enum index");
        }

        return TextureHandle(index, m_textures[index].generation);
    }

	void TextureManager::unloadTexture(TextureHandle handle) {
		if (handle.index >= m_textures.size()) return;
		TextureEntry& entry = m_textures[handle.index];
		if (!entry.valid || entry.generation != handle.generation) return;

		entry.texture.Delete();
		entry.valid = false;
		freeIndices.push(handle.index);
	}

    TextureHandle TextureManager::getTextureHandle(const std::string& name) {
        auto it = std::find_if(
            m_textures.begin(),
            m_textures.end(),
            [&](const TextureEntry& tex) {
                return tex.name == name;
            }
        );

        if (it != m_textures.end()) {
            uint16_t index = static_cast<uint16_t>(std::distance(m_textures.begin(), it));
            return TextureHandle(index, it->generation);
        }
        throw std::runtime_error("Texture with name " + name + " doesn't exist.");
    }

    std::vector<TextureHandle> TextureManager::getLoadedHandles() {
        std::vector<TextureHandle> texs;
        texs.reserve(m_textures.size());


        for (int i = 7; i < m_textures.size(); i++) {
            texs.emplace_back(TextureHandle(i,m_textures[i].generation));
        }

        return texs;
    }

	Texture& TextureManager::getTexture(TextureHandle handle) {
        if (handle.index >= m_textures.size()) {
            throw std::runtime_error(
                "TextureHandle index " + std::to_string(handle.index) +
                " out of range (max: " + std::to_string(m_textures.size()) + ")"
            );
        }

		TextureEntry& entry = m_textures[handle.index];
        if (!entry.valid || entry.generation != handle.generation) {
            throw std::runtime_error(
                "Invalid textureEntry: entry generation " + std::to_string(entry.generation) +
                " handle generation " + std::to_string(handle.generation)
            );
        }

		return entry.texture;
	}

    void TextureManager::loadDefaultTextures() {
        m_defaultTexLoaded = true;

        for (auto& tex : m_defaultTextures) {
            loadTexture(tex, FilterMode::Billinear);
        }
    }
}