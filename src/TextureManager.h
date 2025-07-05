#pragma once
#include <vector>
#include <queue>
#include <memory>
#include "Graphics/Texture.h"
#include <functional>
#include <string>

namespace graphics {
	enum class FilterMode { None = 0, Billinear = 1, Trillinear = 2 };

	struct TextureHandle {
		uint16_t index;
		uint16_t generation;

		TextureHandle(uint16_t index, uint16_t generation) : index{ index }, generation{ generation } {}
		TextureHandle() = default;

		bool operator==(const TextureHandle& other) const {
			return index == other.index && generation == other.generation;
		}
	};

	struct TextureEntry {
		TextureEntry() = default;
		TextureEntry(TextureEntry&&) noexcept = default;
		TextureEntry& operator=(TextureEntry&&) noexcept = default;
		TextureEntry(const TextureEntry&) = delete;
		TextureEntry& operator=(const TextureEntry&) = delete;

		Texture texture;
		uint16_t generation = 0;
		std::string name;
		bool valid = true;
	};

	enum class DefaultTexture {
		Circle,
		Square,
		Capsule,
		IsometricDiamond,
		HexagonFlatTop,
		HexagonPointedTop,
		_9Sliced,
	};

	class TextureManager {
		friend class RenderSystem;
	public:
		static TextureHandle loadTexture(const std::string& path, FilterMode filterMode);
		static TextureHandle getDefaultTexture(DefaultTexture type);
		static void unloadTexture(TextureHandle handle);
		static TextureHandle getTextureHandle(const std::string& name);
		static Texture& getTexture(TextureHandle handle);
		static std::vector<TextureHandle> getLoadedHandles();

	private:
		static void loadDefaultTextures();

		static std::string m_defaultTextures[7];
		static std::vector<TextureEntry> m_textures;
		static std::queue<uint16_t> freeIndices;

		static bool m_defaultTexLoaded;
	};
}

namespace std {
	template<>
	struct hash<graphics::TextureHandle> {
		size_t operator()(graphics::TextureHandle const& h) const noexcept {
			return (static_cast<size_t>(h.index) << 16)
				^ static_cast<size_t>(h.generation);
		}
	};
}