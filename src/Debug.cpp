#include "Debug.h"

namespace engine {
	void Debug::log(const std::string& message, const std::source_location& loc) {
		DebugWindow::addLog(message, 0, loc);
	}
	void Debug::logWarning(const std::string& message, const std::source_location& loc) {
		DebugWindow::addLog(message, 1,loc);
	}
	void Debug::logError(const std::string& message, const std::source_location& loc) {
		DebugWindow::addLog(message, 2, loc);
	}
}