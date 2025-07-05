#pragma once
#include "Core/Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Utils/Time.h"
#include <source_location>
#include <iostream>
#include <string>
#include <queue>

namespace engine {
	class Window; // Forward declaration

	struct LogMessage {
		const std::string k_warningStateMessages[3] = { "Engine Debug.Log()","Engine Debug.LogWarning()", "Engine Debug.LogError()" };

		static const uint8_t MESSAGE = 0;
		static const uint8_t WARNING = 1;
		static const uint8_t ERR = 2;

		std::string message;
		std::string time;
		std::string sourcePath;
		std::string function;
		uint8_t     warningState;
		uint16_t    sourceLine;

		std::string selectedMessage() {
			std::string result = message + "\n"
				+ k_warningStateMessages[warningState] + "\n"
				+ function + " at " + sourcePath + "(" + std::to_string(sourceLine) + ")";
			return result;
		}

		LogMessage(std::string msg,
			std::string t,
			std::string src,
			std::string func,
			uint8_t state,
			uint16_t line)
			: message(std::move(msg))
			, time(std::move(t))
			, sourcePath(std::move(src))
			, function(std::move(func))
			, warningState(state)
			, sourceLine(line)
		{
		}
		// Default Copy/Move funktionieren jetzt optimal
	};

	class DebugWindow {
	public:
		static void addLog(const std::string& log, uint8_t warning, const std::source_location loc);
		static void draw();
		static void setVisible(bool visible) { m_visible = visible; }

	private:
		static void DrawLogLine(uint16_t i, uint16_t realIndex, const float& spacing, bool copy, std::string& clipboard);

		static float m_itemSpacingY;
		static uint16_t s_maxLogsCount;
		static std::deque<LogMessage> m_logs;
		static short selectedIndex;
		static bool m_visible;

		static uint16_t m_messageLogsCount;
		static uint16_t m_warningLogsCount;
		static uint16_t m_errorLogsCount;
	};
}
