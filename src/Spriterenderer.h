#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "Graphics/TextureManager.h"

namespace graphics {
	struct SpriteRenderer {
		TextureHandle texture{0, 0};
		short layer{0};
		glm::vec4 color{1.f};
		static SpriteRenderer create(TextureHandle handle, short layer, glm::vec4 color) {
			return SpriteRenderer{ handle, layer, color };
		}
	};
}