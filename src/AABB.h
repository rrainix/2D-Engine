#pragma once
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include "Components/Transform.h"

namespace graphics {
	struct AABB {
		glm::vec2 min;
		glm::vec2 max;
		static AABB create(const glm::vec2& center, const glm::vec2& halfExtents, float degrees)
		{
			float radians = glm::radians<float>(degrees);
			// Rotationsmatrix erstellen
			glm::mat2 rotation = glm::mat2(
				glm::vec2(cos(radians), sin(radians)),
				glm::vec2(-sin(radians), cos(radians))
			);

			// Ecken definieren
			glm::vec2 corners[4] = {
				glm::vec2(-halfExtents.x, -halfExtents.y),
				glm::vec2(halfExtents.x, -halfExtents.y),
				glm::vec2(halfExtents.x, halfExtents.y),
				glm::vec2(-halfExtents.x, halfExtents.y)
			};

			// Rotierte Ecken berechnen
			glm::vec2 rotatedCorners[4];
			for (int i = 0; i < 4; ++i) {
				rotatedCorners[i] = center + rotation * corners[i];
			}

			// Jetzt min/max finden
			glm::vec2 min = rotatedCorners[0];
			glm::vec2 max = rotatedCorners[0];
			for (int i = 1; i < 4; ++i) {
				min = glm::min(min, rotatedCorners[i]);
				max = glm::max(max, rotatedCorners[i]);
			}

			return { min, max };
		}
		static AABB create(const glm::vec2& center, const glm::vec2& halfExtents)
		{
			glm::vec2 min = center - halfExtents;
			glm::vec2 max = center + halfExtents;
			return { min, max };
		}
		static AABB create(engine::Transform2D& transform) {
			if (isAxisAligned(transform.rotation)) {
				return AABB::create(transform.position, transform.scale / 2.f);
			}
			else {
				return AABB::create(transform.position, transform.scale / 2.f , transform.getRotationDegrees());
			}
		}
		static bool intersects(AABB a, AABB b) {
			return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
				(a.min.y <= b.max.y && a.max.y >= b.min.y);
		}
		static bool contains(AABB a, const glm::vec2& point) {
			return (point.x >= a.min.x && point.x <= a.max.x) &&
				(point.y >= a.min.y && point.y <= a.max.y);
		}

		glm::vec2 scale() const {
			float x = max.x - min.x;
			float y = max.y - min.y;
			return { x, y };
		}

		static bool isAxisAligned(float radians) {
			constexpr float angles[] = {
				0.0f,
				glm::half_pi<float>(),       // 90°
				glm::pi<float>(),            // 180°
				3 * glm::half_pi<float>(),   // 270°
				glm::two_pi<float>()         // 360°
			};

			for (float angle : angles) {
				if (radians == angle)
					return true;
			}

			return false;
		}
	};
}