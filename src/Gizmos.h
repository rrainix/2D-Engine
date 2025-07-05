#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Graphics/DebugRenderer.h"  // hier den richtigen Pfad

namespace graphics {
	struct Box { glm::vec2 center, halfExtents; float radiant; glm::vec4 color; };
	struct Circle { glm::vec2 center; float radius; int segments; glm::vec4 color; };
	struct Line { glm::vec2 start; glm::vec2 end; glm::vec4 color; };

	class Gizmos {
		friend class GizmosRenderSystem;

	public:
		static inline void drawBox(glm::vec2 center, glm::vec2 scale, float degrees) {
			if (!enabled || s_registeredVertices + k_boxVertices >= s_maxVertices)
				return;

			float radiant = glm::radians<float>(degrees);
			AABB boxAABB = AABB::isAxisAligned(radiant) ? AABB::create(center, scale / 2.f) : AABB::create(center, scale / 2.f, degrees);
			if (!AABB::intersects(boxAABB, camViewportAABB))
				return;

			s_registeredVertices += k_boxVertices;
			s_boxes.emplace_back(Box{ center, scale / 2.f, glm::radians<float>(degrees), color });
		}

		static inline void drawLine(glm::vec2 start, glm::vec2 end) {
			if (!enabled || s_registeredVertices + k_lineVertices >= s_maxVertices)
				return;

			if (!AABB::contains(camViewportAABB, start) && !AABB::contains(camViewportAABB, end))
				return;

			s_registeredVertices += k_lineVertices;
			s_lines.emplace_back(Line{ start, end, color });
		}
		static inline void drawCircle(glm::vec2 center, float radius, int segments = 32) {
			if (!enabled || s_registeredVertices + segments >= s_maxVertices)
				return;

			AABB circleAABB = AABB::create(center, glm::vec2{ radius });
			if (!AABB::intersects(circleAABB, camViewportAABB))
				return;

			s_registeredVertices += segments;
			s_circles.emplace_back(Circle{ center,radius,segments, color });
		}

		static inline size_t maxVertices() { return s_maxVertices; }
		static inline size_t registeredVertices() { return s_registeredVertices; }

		static  inline void clear() { s_boxes.clear(); s_lines.clear(); s_circles.clear(); s_registeredVertices = 0; }
		static  glm::vec4 color;
		static float lineWidth;
		static bool enabled;

		static bool drawCollider;
		static bool aabb;

	private:


		static AABB camViewportAABB;
		static std::vector<Box> s_boxes;
		static std::vector<Circle> s_circles;
		static std::vector<Line> s_lines;

		static const size_t k_boxVertices = 4;
		static const size_t k_lineVertices = 1;

		static size_t s_maxVertices;
		static size_t s_registeredVertices;

		friend class RenderSystem;
	};
}