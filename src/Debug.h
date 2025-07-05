#pragma once
#include <string>
#include "Core/DebugWindow.h"

namespace engine {
	class Debug {
	public:
		static void log(const std::string& message, const std::source_location& loc = std::source_location::current());
		static void logWarning(const std::string& message, const std::source_location& loc = std::source_location::current());
		static void logError(const std::string& message, const std::source_location& loc = std::source_location::current());
	};
}