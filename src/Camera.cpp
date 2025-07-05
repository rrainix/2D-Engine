#include"Camera.h"

namespace graphics {
    Camera* Camera::m_mainCamera = nullptr;

    Camera::Camera(int width, int height, engine::Transform2D* transform)
        : m_width{ width }, m_height{ height }, transform{ transform } {
        updateProjection();
        updateViewYXZ();   
        setNearFar(-0.1f, 1000);
    }

	Camera::~Camera() {};

    void Camera::orthographicSize(float size) {
        m_orthographicSize = size;
        updateProjection();
    }

    void Camera::updateProjection() {
        setOrthographicProjection();
    }

    void Camera::setNearFar(float newNear, float newFar) {
        m_near = newNear;
        m_far = newFar;
        updateProjection();
    }

    void Camera::updateViewYXZ() {
        float theta = transform->rotation;
        // Für den View-Matrix sollte man um -theta rotieren:
        float c = glm::cos(-theta);
        float s = glm::sin(-theta);

        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0][0] = c;
        rotation[0][1] = -s;
        rotation[1][0] = s;
        rotation[1][1] = c;

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-transform->position, 0.0f));

        m_viewMatrix = rotation * translation;
    }

    void Camera::setOrthographicProjection() {
        float aspect = static_cast<float>(m_width) / m_height;
        float halfHeight = m_orthographicSize;
        float halfWidth = m_orthographicSize * aspect;
        float left = -halfWidth;
        float right = halfWidth;
        float bottom = -halfHeight;  // Unten ist der kleinere Y-Wert
        float top = halfHeight;      // Oben ist der größere Y-Wert

        m_projectionMatrix = glm::mat4{ 1.0f };
        m_projectionMatrix[0][0] = 2.f / (right - left);
        m_projectionMatrix[1][1] = 2.f / (top - bottom);
        m_projectionMatrix[2][2] = -2.f / (m_far - m_near);  // für [-1, 1]-Mapping
        m_projectionMatrix[3][0] = -(right + left) / (right - left);
        m_projectionMatrix[3][1] = -(top + bottom) / (top - bottom);
        m_projectionMatrix[3][2] = -(m_far + m_near) / (m_far - m_near);
    }
}
