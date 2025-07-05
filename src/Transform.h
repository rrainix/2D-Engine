#pragma once
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <string>


namespace engine {
	struct Transform2D {
		Transform2D() = default;

		static Transform2D FromPosition(glm::vec2 position) {
			Transform2D transform;
			transform.position = position;
			return transform;
		}
		static Transform2D FromPositionRotation(glm::vec2 position, float degrees) {
			Transform2D transform;
			transform.position = position;
			transform.rotation = glm::radians(degrees);
			return transform;
		}
		static Transform2D FromPositionScaleRotation(glm::vec2 position, glm::vec2 scale, float degrees) {
			Transform2D transform;
			transform.position = position;
			transform.scale = scale;
			transform.rotation = glm::radians(degrees);;
			return transform;
		}
		static Transform2D FromPositionScale(glm::vec2 position, glm::vec2 scale) {
			Transform2D transform;
			transform.position = position;
			transform.scale = scale;
			return transform;
		}

		static Transform2D FromScale(glm::vec2 scale) {
			Transform2D transform;
			transform.scale = scale;
			return transform;
		}
		static Transform2D FromScaleRotation(glm::vec2 scale, float degrees) {
			Transform2D transform;
			transform.scale = scale;
			transform.rotation = glm::radians(degrees);
			return transform;
		}

		static Transform2D FromRotation(float degrees) {
			Transform2D transform;
			transform.rotation = glm::radians(degrees);
			return transform;
		}

		float getRotationDegrees() { return glm::degrees(rotation); }

		glm::vec2 position{0.f};        // 2D position (X, Y)
		glm::vec2 scale{ 1.f, 1.f };   // 2D scale (X, Y)
		float rotation{0.f};            // Rotation angle in radians (around Z-axis)
		bool dirtyPosOrRot{ false }, dirtyScale{ false };

		void setPosition(const glm::vec2& p) {
			position = p;
			dirtyPosOrRot = true;
		}
		void setScale(const glm::vec2& s) {
			scale = s;
			dirtyScale = true;
		}
		void setRotation(float degrees) {
			rotation = glm::radians(degrees);
			dirtyPosOrRot = true;
		}

		b2Rot b2Rotation() const {
			return  b2Rot(std::cos(rotation), std::sin(rotation));
		}

		glm::mat3 mat3() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			// Rotation matrix in 3x3 form:
			glm::mat3 rotMatrix{
				{ c,  s, 0.0f },
				{ -s, c, 0.0f },
				{ 0.0f, 0.0f, 1.0f }
			};

			// Scaling matrix in 3x3 form:
			glm::mat3 scaleMat{
				{ scale.x, 0.0f,   0.0f },
				{ 0.0f,   scale.y, 0.0f },
				{ 0.0f,   0.0f,    1.0f }
			};

			// Corrected Translation matrix in 3x3 form:
			glm::mat3 transMat{
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ position.x, position.y, 1.0f }
			};

			// Combine them: translation * rotation * scale
			return transMat * rotMatrix * scaleMat;
		}

		std::string ToString(bool detailed = true) {
			if (detailed)
			{
				return
					"{ " + std::to_string(position.x) + ", " + std::to_string(position.y) + " }" + "\n" +
					"{ " + std::to_string(scale.x) + ", " + std::to_string(scale.y) + " }" + "\n" + 
					"{ " + std::to_string(rotation) + " }";
			}
			else {
				return 	"{ " + std::to_string(position.x) + ", " + std::to_string(position.y) + " }";
			}
		}
	};
}
