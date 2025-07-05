#pragma once

#include "Components/Transform.h";
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include"Shader.h"
#include <array>
#include "Utils/AABB.h"

namespace graphics {
	class Camera
	{

	public:
		Camera(int width, int height, engine::Transform2D* transform);

		~Camera();

		void updateProjection();
		void updateViewYXZ();
		void setNearFar(float newNear, float newFar);
		
		void updateViewSize(int width, int height) { m_width = width; m_height = height; }
		void setOrthographicProjection();
		void orthographicSize(float size);

		engine::Transform2D* transform;

		glm::mat4 viewProjection() const { return m_projectionMatrix * m_viewMatrix; }
		glm::mat4& viewMatrix() { return m_viewMatrix; }
		glm::mat4& projectionMatrix() { return m_projectionMatrix; }
		float orthographicSize()const { return m_orthographicSize; }

		glm::vec2 worldViewPort() const {
			float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
			float worldHeight = 2.0f * m_orthographicSize;
			float worldWidth = worldHeight * aspectRatio;

			return { worldWidth, worldHeight };
		}
		glm::vec2 screenToWorld(glm::vec2 pos) const {
			float ndcX = pos.x / m_width * 2.0f - 1.0f;
			float ndcY = pos.y / m_height * 2.0f + 1.0f;

			glm::vec2 worldViewport = worldViewPort();

			glm::vec2 worldPos;
			worldPos.x = ndcX * (worldViewport.x / 2.0f) + transform->position.x;
			worldPos.y = ndcY * (worldViewport.y / 2.0f) + transform->position.y;

			return worldPos;
		}
		AABB viewportAABB() {
			glm::vec2 viewPort = worldViewPort();
			return AABB::create(transform->position, viewPort / 2.f);
		}

		static Camera* main() {
			if (m_mainCamera == nullptr) {
				throw std::runtime_error("There is no main camera.");
			}
			return m_mainCamera;
		}
		void setMain() {
			m_mainCamera = this;
		}

		int width() const { return m_width; }
		int height() const { return m_height; }
		static Camera* m_mainCamera;
	private:


		glm::mat4 m_projectionMatrix{ 1.f };
		glm::mat4 m_viewMatrix{ 1.f };

		int m_width, m_height;
		float m_orthographicSize = 5.0f;
		float m_fovy{ glm::radians(60.f) };
		float m_near{ 0.1f };
		float m_far{ 100.0f };
	};
}