#include "Gizmos.h"

namespace graphics {
	std::vector<Box> Gizmos::s_boxes;
	std::vector<Circle> Gizmos::s_circles;
	std::vector<Line> Gizmos::s_lines;
	size_t Gizmos::s_maxVertices = 100000;
	size_t Gizmos::s_registeredVertices = 0;
	AABB Gizmos::camViewportAABB = AABB();
	bool Gizmos::drawCollider = false;
	bool Gizmos::aabb = false;

	float Gizmos::lineWidth = 1.0f;
	bool Gizmos::enabled = true;
	glm::vec4 Gizmos::color = { 0.f, 1.f, 0.f, 1.f };
}
