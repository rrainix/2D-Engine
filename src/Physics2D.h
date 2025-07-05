#pragma once
#include "Core/Scene.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/collision.h>
#include "Components/Rigidbody2D.h"
#include "Components/Collider.h"
#include <algorithm>
#include <vector>
#include <optional>
#include <cstdint>

namespace engine {
	struct RaycastHit2D {
		entt::entity entity;
		glm::vec2 point;
		glm::vec2 normal;
		float distance;
	};

	enum class OverlapMode {
		First,   ///< gib den allerersten Treffer zurück
		Nearest  ///< gib den Treffer mit minimalem Abstand zum Zentrum zurück
	};

	class Physics2D {
	public:
		static std::optional<entt::entity> overlapCircle(Scene& scene, const glm::vec2& center, float radius, OverlapMode mode) {
			auto& phys = scene.physicsWorld();
			b2WorldId world = phys.m_worldId;

			// ShapeProxy für einen Kreis: single point + radius
			b2ShapeProxy proxy{};
			proxy.count = 1;
			proxy.points[0] = { center.x, center.y };
			proxy.radius = radius;

			b2QueryFilter filter = b2DefaultQueryFilter();

			struct Qb {
				Scene* scene;
				glm::vec2           ctr;
				OverlapMode         mode;
				std::optional<entt::entity> first;
				std::optional<entt::entity> nearest;
				float               bestDist2 = std::numeric_limits<float>::max();

				static bool Report(b2ShapeId sId, void* ctx) {
					auto* self = static_cast<Qb*>(ctx);

					// Entity recovern
					b2BodyId bId = b2Shape_GetBody(sId);
					void* ud = b2Body_GetUserData(bId);
					entt::entity h = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));

					if (self->mode == OverlapMode::First) {
						self->first = h;
						return false; // sofort abbrechen
					}

					// Nearest: Abstand² Body-Position ↔ Kreis-Mittelpunkt
					b2Transform xf = b2Body_GetTransform(bId);
					float dx = xf.p.x - self->ctr.x;
					float dy = xf.p.y - self->ctr.y;
					float d2 = dx * dx + dy * dy;
					if (d2 < self->bestDist2) {
						self->bestDist2 = d2;
						self->nearest = h;
					}
					return true; // weitersuchen
				}
			} qb{ &scene, center, mode, std::nullopt, std::nullopt };

			b2World_OverlapShape(world, &proxy, filter, Qb::Report, &qb);

			return (mode == OverlapMode::First ? qb.first : qb.nearest);
		}
		static std::optional<entt::entity> overlapBox(Scene& scene, const glm::vec2& center, const glm::vec2& halfExtents, float degrees, OverlapMode mode) {
			auto& phys = scene.physicsWorld();
			b2WorldId world = phys.m_worldId;
			float radians = glm::radians<float>(degrees);

			// Eckpunkte des OBB in Weltkoordinaten
			glm::vec2 corners[4] = {
				{ halfExtents.x,  halfExtents.y},
				{-halfExtents.x,  halfExtents.y},
				{-halfExtents.x, -halfExtents.y},
				{ halfExtents.x, -halfExtents.y}
			};
			glm::mat2 rot = {
				{ cos(radians), -sin(radians) },
				{ sin(radians),  cos(radians) }
			};

			b2ShapeProxy proxy{};
			proxy.count = 4;
			proxy.radius = 0.0f;
			for (int i = 0; i < 4; ++i) {
				glm::vec2 w = rot * corners[i] + center;
				proxy.points[i] = { w.x, w.y };
			}

			b2QueryFilter filter = b2DefaultQueryFilter();

			struct Qb {
				Scene* scene;
				glm::vec2           ctr;
				OverlapMode         mode;
				std::optional<entt::entity> first;
				std::optional<entt::entity> nearest;
				float               bestDist2 = std::numeric_limits<float>::max();

				static bool Report(b2ShapeId sId, void* ctx) {
					auto* self = static_cast<Qb*>(ctx);

					// Entity recovern
					b2BodyId bId = b2Shape_GetBody(sId);
					void* ud = b2Body_GetUserData(bId);
					entt::entity h = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));

					if (self->mode == OverlapMode::First) {
						self->first = h;
						return false;
					}

					// Nearest: Abstand² Body-Position ↔ OBB-Zentrum
					b2Transform xf = b2Body_GetTransform(bId);
					float dx = xf.p.x - self->ctr.x;
					float dy = xf.p.y - self->ctr.y;
					float d2 = dx * dx + dy * dy;
					if (d2 < self->bestDist2) {
						self->bestDist2 = d2;
						self->nearest = h;
					}
					return true;
				}
			} qb{ &scene, center, mode, std::nullopt, std::nullopt };

			b2World_OverlapShape(world, &proxy, filter, Qb::Report, &qb);

			return (mode == OverlapMode::First ? qb.first : qb.nearest);
		}
		static std::optional<RaycastHit2D> raycast(Scene& scene, const glm::vec2 origin, glm::vec2 direction, float maxDistance) {
			auto& phys = scene.physicsWorld();
			b2WorldId world = phys.m_worldId;

			// Ursprung und Translation berechnen
			b2Vec2 o{ origin.x, origin.y };
			glm::vec2 nd = glm::normalize(direction);
			b2Vec2 t{ nd.x * maxDistance, nd.y * maxDistance };

			// Standard-Filter (alle Kollisionsgruppen)
			b2QueryFilter filter = b2DefaultQueryFilter();

			// C-API: hole den nächsten Treffer
			b2RayResult r = b2World_CastRayClosest(world, o, t, filter);  // :contentReference[oaicite:0]{index=0}

			if (!b2Shape_IsValid(r.shapeId))
				return std::nullopt;

			// Entity aus UserData recovern
			b2BodyId   bId = b2Shape_GetBody(r.shapeId);
			void* ud = b2Body_GetUserData(bId);
			uintptr_t i = reinterpret_cast<uintptr_t>(ud);
			entt::entity e = static_cast<entt::entity>(i);

			RaycastHit2D hit;
			hit.entity = e;
			hit.point = { r.point.x, r.point.y };
			hit.normal = { r.normal.x, r.normal.y };
			hit.distance = r.fraction * maxDistance;
			return hit;
		}
		static std::vector<entt::entity> overlapCircleAll(Scene& scene, const glm::vec2& center, float radius) {
			auto& phys = scene.physicsWorld();
			b2WorldId world = phys.m_worldId;

			// ShapeProxy für Kreis
			b2ShapeProxy proxy{};
			proxy.count = 1;
			proxy.points[0] = { center.x, center.y };
			proxy.radius = radius;

			b2QueryFilter filter = b2DefaultQueryFilter();

			std::vector<entt::entity> results;
			struct Cb {
				std::vector<entt::entity>* out;
				static bool Report(b2ShapeId sId, void* ctx) {
					auto* self = static_cast<Cb*>(ctx);
					b2BodyId bId = b2Shape_GetBody(sId);
					void* ud = b2Body_GetUserData(bId);
					entt::entity h = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
					self->out->push_back(h);
					return true; // true = weiter suchen
				}
			} cb{ &results };

			b2World_OverlapShape(world, &proxy, filter, Cb::Report, &cb);  // C-API

			return results;
		}
		static std::vector<entt::entity> overlapBoxAll(Scene& scene, const glm::vec2& center, const glm::vec2& halfExtents, float degrees) {
			auto& phys = scene.physicsWorld();
			b2WorldId world = phys.m_worldId;
			float radians = glm::radians<float>(degrees);

			// Eckpunkte des OBB in Weltkoordinaten
			glm::vec2 corners[4] = {
				{ halfExtents.x,  halfExtents.y},
				{-halfExtents.x,  halfExtents.y},
				{-halfExtents.x, -halfExtents.y},
				{ halfExtents.x, -halfExtents.y}
			};
			glm::mat2 rot = {
				{ cos(radians), -sin(radians) },
				{ sin(radians),  cos(radians) }
			};

			b2ShapeProxy proxy{};
			proxy.count = 4;
			proxy.radius = 0.0f;
			for (int i = 0; i < 4; ++i) {
				glm::vec2 w = rot * corners[i] + center;
				proxy.points[i] = { w.x, w.y };
			}

			b2QueryFilter filter = b2DefaultQueryFilter();

			std::vector<entt::entity> results;
			struct Cb {
				std::vector<entt::entity>* out;
				static bool Report(b2ShapeId sId, void* ctx) {
					auto* self = static_cast<Cb*>(ctx);
					b2BodyId bId = b2Shape_GetBody(sId);
					void* ud = b2Body_GetUserData(bId);
					entt::entity h = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
					self->out->push_back(h);
					return true;
				}
			} cb{ &results };

			b2World_OverlapShape(world, &proxy, filter, Cb::Report, &cb);

			return results;
		}

		static entt::entity fromCollider(Collider collider) {
			void* ud = b2Body_GetUserData(collider.m_bodyId);
			return static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
		}

		static entt::entity fromRigidbody(Rigidbody2D rb) {
			void* ud = b2Body_GetUserData(rb.m_bodyId);
			return static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
		}

		static entt::entity fromBodyId(b2BodyId bodyId) {
			void* ud = b2Body_GetUserData(bodyId);
			return static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
		}

		static entt::entity fromShapeId(b2ShapeId shapeId) {
			void* ud = b2Body_GetUserData(b2Shape_GetBody(shapeId));
			return static_cast<entt::entity>(reinterpret_cast<uintptr_t>(ud));
		}
	};
}