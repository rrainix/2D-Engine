#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include "Utils/Input.h"

namespace engine {
	enum class WindowMode { FULLSCREEN, WINDOWED_MAXIMIZED, WINDOWED_FREE };

	class Window {
	public:

		/// Constructs a new Window with the given dimensions, title, and display mode.
		Window(int width, int height, const std::string& title, WindowMode windowMode);
		/// Constructs a new Window with the given dimensions, title, and display mode.
		Window(int width, int height, const std::string& title, WindowMode windowMode, bool focus);

		/// Calls the destroy() method.
		~Window();


		/// Initializes GLFW window API.
		static void init() { glfwInit(); }

		GLFWwindow* glfwWindow() const { return m_window; }
		float aspect() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

		/// Returns the size of this Window instance in pixels.
		glm::ivec2 size() const { return { m_width, m_height }; }

		/// Limits the fps of all windows to the monitor HZ refresh rate.
		static void vsync(bool enabled) { glfwSwapInterval(enabled ? 1 : 0); };

		/// Makes this window resizeable if enabled = true, else nonresizeable.
		void setWindowResizeable(bool enabled) { glfwWindowHint(GLFW_RESIZABLE, enabled ? GLFW_TRUE : GLFW_FALSE); }

		/// Makes this window moveable if enabled = true, else hides it's decoration.
		void setWindowMoveable(bool enabled) { glfwWindowHint(GLFW_DECORATED, enabled ? GLFW_TRUE : GLFW_FALSE); }

		/// Maximizes this window.
		void maximizeWindow();

		/// Minimizes this window.
		void minimizeWindow();

		/// Returns weather this window is maximized or not.
		bool isMaximized() const;

		/// Returns weather this window is minimized or not.
		bool isMinimized() const;

		/// Centers the window in the middle of the screen.
		void centerWindow();

		/// Focuses the window.
		void focusWindow();

		//Returns the last created Window
		static Window& activeWindow() { return *s_activeWindow; }

		/// Returns the width of this Window instance in pixels.
		int width()const { return m_width; }
		/// Returns the heght of this Window instance in pixels.
		int height() const { return m_height; }

		/// Destroys this window and makes it unuseable.
		void destroy();

	private:
		// Used for initializing and creating the window.
		void initWindow();

		// Called when windows size has been changed.
		void updateViewport();

		// Called for updateing the windows size to m_width and m_height values.
		void updateWindowSize();

		// Has to be called/checked every gameloop frame.
		bool shouldClose() const { return glfwWindowShouldClose(m_window); }
		void swapBuffers() const { glfwSwapBuffers(m_window); }

		// Used to detected any window resizeing, for camera, screen and render settings.
		bool isWindowResized() const { return m_windowResized; }
		void resetWindowResizedFlag() { m_windowResized = false; }


		// Returns the screens center position.
		glm::ivec2 screenCenter() const;

		// Callbacks
		static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		static void windowResizedCallback(GLFWwindow* window, int width, int height);


		static Window* s_activeWindow;

		GLFWwindow* m_window = nullptr;
		GLFWmonitor* m_monitor = nullptr;
		const GLFWvidmode* k_mode = nullptr;


		int m_width, m_height;
		std::string m_title;
		bool m_windowResized = false;
		WindowMode m_windowMode;

		friend class Application;
		friend class DebugWindow;
	};
}