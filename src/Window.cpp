#include "Window.h"

namespace engine {
	Window* Window::s_activeWindow = nullptr;

	Window::Window(int width, int height, const std::string& title, WindowMode windowMode)
		: m_width{ width }, m_height{ height }, m_title{ title }, m_windowMode{ windowMode } 
	{
		initWindow();
	}

	Window::Window(int width, int height, const std::string& title, WindowMode windowMode, bool focus)
		: m_width{ width }, m_height{ height }, m_title{ title }, m_windowMode{ windowMode } {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Setup Fenstergrößenhinweise
		if (m_windowMode == WindowMode::FULLSCREEN) glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		if (m_windowMode != WindowMode::WINDOWED_FREE) glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_monitor = glfwGetPrimaryMonitor();
		k_mode = glfwGetVideoMode(m_monitor);

		// Fenster erstellen je nach Modus
		if (m_windowMode == WindowMode::FULLSCREEN) {
			m_window = glfwCreateWindow(k_mode->WIDTH, k_mode->HEIGHT, m_title.c_str(), m_monitor, nullptr);
			maximizeWindow();
		}
		else if (m_windowMode == WindowMode::WINDOWED_MAXIMIZED) {
			m_window = glfwCreateWindow(k_mode->WIDTH, k_mode->HEIGHT, m_title.c_str(), nullptr, nullptr);
			maximizeWindow();
		}
		else { // WINDOWED_FREE
			m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
			centerWindow();
		}

		assert(m_window && "Failed to create window!");

		if (focus) {
			glfwMakeContextCurrent(m_window);
			glfwSetWindowUserPointer(m_window, this);
		}

		glfwSetScrollCallback(m_window, scrollCallback);

		if (m_windowMode == WindowMode::WINDOWED_FREE) {
			glfwSetFramebufferSizeCallback(m_window, windowResizedCallback);
		}

		glfwSwapInterval(vsync ? 1 : 0); // V-Sync
		updateWindowSize();

		if(focus)
		s_activeWindow = this;
	}

	Window::~Window() {
		destroy();
	}

	void Window::destroy() {
		glfwDestroyWindow(m_window);
	}

	void Window::initWindow() {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Setup Fenstergrößenhinweise
		if (m_windowMode == WindowMode::FULLSCREEN) glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		if (m_windowMode != WindowMode::WINDOWED_FREE) glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_monitor = glfwGetPrimaryMonitor();
		k_mode = glfwGetVideoMode(m_monitor);

		// Fenster erstellen je nach Modus
		if (m_windowMode == WindowMode::FULLSCREEN) {
			m_window = glfwCreateWindow(k_mode->WIDTH, k_mode->HEIGHT, m_title.c_str(), m_monitor, nullptr);
			maximizeWindow();
		}
		else if (m_windowMode == WindowMode::WINDOWED_MAXIMIZED) {
			m_window = glfwCreateWindow(k_mode->WIDTH, k_mode->HEIGHT, m_title.c_str(), nullptr, nullptr);
			maximizeWindow();
		}
		else { // WINDOWED_FREE
			m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
			centerWindow();
		}

		assert(m_window && "Failed to create window!");

		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetScrollCallback(m_window, scrollCallback);

		if (m_windowMode == WindowMode::WINDOWED_FREE) {
			glfwSetFramebufferSizeCallback(m_window, windowResizedCallback);
		}

		glfwSwapInterval(vsync ? 1 : 0); // V-Sync
		updateWindowSize();

		s_activeWindow = this;
	}

	void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		engine::Input::s_scrollValue = static_cast<float>(yoffset);
	}

	void Window::centerWindow() {
		glm::ivec2 m_screenCenter = screenCenter();

		int windowWidth, windowHeight;
		glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
		int posX = m_screenCenter.x - windowWidth / 2;
		int posY = m_screenCenter.y - windowHeight / 2;
		glfwSetWindowPos(m_window, posX, posY);
	}
	void Window::focusWindow() {
		glfwFocusWindow(m_window);
	}

	glm::ivec2 Window::screenCenter() const {
		return glm::ivec2(k_mode->WIDTH / 2, k_mode->HEIGHT / 2);
	}

	void Window::maximizeWindow() {
		glfwMaximizeWindow(m_window);
		glfwGetWindowSize(m_window, &m_width, &m_height);
	}

	void Window::minimizeWindow() {
		glfwIconifyWindow(m_window);
		glfwGetWindowSize(m_window, &m_width, &m_height);
	}

	bool Window::isMaximized() const {
		if (m_windowMode == WindowMode::FULLSCREEN) {
			return true;
		}
		return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
	}

	bool Window::isMinimized() const {
		if (m_windowMode == WindowMode::FULLSCREEN) {
			return true;
		}
		return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
	}

	void Window::windowResizedCallback(GLFWwindow* window, int WIDTH, int HEIGHT) {
		auto _window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		_window->m_windowResized = true;
		_window->m_width = WIDTH;
		_window->m_height = HEIGHT;
		_window->updateViewport();
	}

	void Window::updateViewport() {
		glViewport(0, 0, m_width, m_height);
	}

	void Window::updateWindowSize() {
		glfwGetWindowSize(m_window, &m_width, &m_height);
	}
}