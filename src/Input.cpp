#include "Input.h"

namespace engine {
	GLFWwindow* Input::s_window = nullptr;

	bool Input::s_currentKeyStates[Input::k_keyCount] = { false };
	bool Input::s_previousKeyStates[Input::k_keyCount] = { false };
	bool Input::s_currentMouseButtonStates[Input::k_mouseButtonCount] = { false };
	bool Input::s_previousMouseButtonStates[Input::k_mouseButtonCount] = { false };
	bool Input::s_active = true;

	float Input::s_scrollValue = { 0 };
	glm::vec2 Input::s_mouseAxis = { 0, 0 };
	glm::vec2 Input::s_mousePosition{ 0.f };

	void Input::updateKeyStates() {
		if (!s_active)return;

		std::copy(std::begin(s_currentKeyStates), std::end(s_currentKeyStates), std::begin(s_previousKeyStates));
		std::copy(std::begin(s_currentMouseButtonStates), std::end(s_currentMouseButtonStates), std::begin(s_previousMouseButtonStates));

		for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
			s_currentKeyStates[key] = (glfwGetKey(s_window, key) == GLFW_PRESS);
		}

		for (int key = 0; key <= 7; ++key) {
			s_currentMouseButtonStates[key] = (glfwGetMouseButton(s_window, key) == GLFW_PRESS);
		}


		double x, y;
		glfwGetCursorPos(s_window, &x, &y);
		glm::vec2 currentMouseAxis = { x, -y };

		s_mousePosition = currentMouseAxis;
		static glm::vec2 lastMousePos = currentMouseAxis;

		s_mouseAxis = lastMousePos - currentMouseAxis;
		lastMousePos = currentMouseAxis;

		static float lastScrollvalue = 0;
		if (lastScrollvalue == s_scrollValue)
			s_scrollValue = 0;

		lastScrollvalue = s_scrollValue;
	}

	bool Input::getKey(KeyCode keyCode) {
		int key = static_cast<int>(keyCode);
		if (key < 0 || key > GLFW_KEY_LAST) return false;
		return s_currentKeyStates[key];
	}

	bool Input::getKeyDown(KeyCode keyCode) {
		int key = static_cast<int>(keyCode);
		if (key < 0 || key > GLFW_KEY_LAST) return false;
		return s_currentKeyStates[key] && !s_previousKeyStates[key];
	}

	bool Input::getKeyUp(KeyCode keyCode) {
		int key = static_cast<int>(keyCode);
		if (key < 0 || key > GLFW_KEY_LAST) return false;
		return !s_currentKeyStates[key] && s_previousKeyStates[key];
	}

	bool Input::getMouseButton(int mouseButton) {
		if (mouseButton < 0 || mouseButton >= k_mouseButtonCount) return false;
		return s_currentMouseButtonStates[mouseButton];
	}

	bool Input::getMouseButtonDown(int mouseButton) {
		if (mouseButton < 0 || mouseButton >= k_mouseButtonCount) return false;
		return s_currentMouseButtonStates[mouseButton] && !s_previousMouseButtonStates[mouseButton];
	}

	bool Input::getMouseButtonUp(int mouseButton) {
		if (mouseButton < 0 || mouseButton >= k_mouseButtonCount) return false;
		return !s_currentMouseButtonStates[mouseButton] && s_previousMouseButtonStates[mouseButton];
	}
}