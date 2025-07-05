#include "DebugWindow.h"
#include "Utils/Debug.h"
#include <cstdlib>
#include <windows.h>
#include <filesystem>

namespace engine {
	float DebugWindow::m_itemSpacingY = 2.0f;
	uint16_t DebugWindow::s_maxLogsCount = 3000;
	std::deque<LogMessage> DebugWindow::m_logs = {};
	bool DebugWindow::m_visible = true;
	short DebugWindow::selectedIndex = -1;
	uint16_t DebugWindow::m_messageLogsCount = 0;
	uint16_t DebugWindow::m_warningLogsCount = 0;
	uint16_t DebugWindow::m_errorLogsCount = 0;

	std::string getCurrentTimeString() {
		using namespace std::chrono;
		// 1) Hol dir jetzt und Millisekunden
		auto now = system_clock::now();
		auto epoch = now.time_since_epoch();
		auto ms = duration_cast<milliseconds>(epoch) % 1000;
		auto secs = duration_cast<seconds>(epoch);
		std::time_t t = static_cast<std::time_t>(secs.count());

		// 2) Lokale Zeit in tm packen (thread-sicher)
		std::tm tm;
#ifdef _WIN32
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		// 3) Schreibe Timestamp in einen kleinen Stack-Buffer
		char buffer[16];
		// "%H:%M:%S" + null = 9 Zeichen
		std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);

		// 4) Füge die Millisekunden mit snprintf an (3 Stellen, führende Nullen)
		char out[20];
		std::snprintf(out, sizeof(out), "%s:%03d", buffer, static_cast<int>(ms.count()));

		// 5) Zurück als std::string (eine Allokation, unvermeidbar)
		return std::string(out);
	}

	void DebugWindow::addLog(const std::string& message,
		uint8_t warning,
		const std::source_location loc)
	{
		if (warning == LogMessage::MESSAGE) {
			m_messageLogsCount++;
		}
		else if (warning == LogMessage::WARNING) {
			m_warningLogsCount++;
		}

		else if (warning == LogMessage::ERR) {
			m_errorLogsCount++;
		}

		// 1) Zeit nur einmal holen
		std::string t = getCurrentTimeString();

		// 2) Emplace direkt mit Moves (keine temporären C‑Strings)
		m_logs.emplace_back(
			message,                // copy
			std::move(t),           // move
			loc.file_name(),        // string ctor
			loc.function_name(),    // string ctor
			warning,
			static_cast<uint16_t>(loc.line())
		);
		if (m_logs.size() > s_maxLogsCount)
		{
			uint8_t warning = m_logs.front().warningState;

			if (warning == LogMessage::MESSAGE)
				m_messageLogsCount--;

			else if (warning == LogMessage::ERR)
				m_errorLogsCount--;

			else if (warning == LogMessage::WARNING)
				m_warningLogsCount--;

			m_logs.pop_front();
		}
	}

	void DebugWindow::DrawLogLine(uint16_t i, uint16_t realIndex, const float& spacing, bool copy, std::string& clipboard) {
		const auto& line = m_logs[i];
		const std::string fullTxt = "[" + std::string(line.time) + "] " + std::string(line.message);
		bool selected = i == selectedIndex;

		ImGui::PushID(i);

		float contentWidth = ImGui::GetContentRegionAvail().x;
		float wrapX = ImGui::GetCursorPosX() + contentWidth;

		// Kalkuliere Höhe des Textes
		ImGui::PushTextWrapPos(wrapX);
		ImVec2 textSize = ImGui::CalcTextSize(fullTxt.c_str(), nullptr, false, contentWidth);
		textSize.y += spacing;
		ImGui::PopTextWrapPos();

		ImVec2 itemStart = ImGui::GetCursorScreenPos();
		ImVec2 bgStart = ImVec2(itemStart.x, itemStart.y - spacing);
		ImVec2 itemEnd = ImVec2(itemStart.x + contentWidth, itemStart.y + textSize.y);


		ImU32 bgColor = (realIndex % 2 == 0) ? IM_COL32(40, 40, 40, 255) : IM_COL32(60, 60, 60, 255);
		ImGui::GetWindowDrawList()->AddRectFilled(bgStart, itemEnd, bgColor);

		bool hovered = ImGui::IsMouseHoveringRect(bgStart, itemEnd);
		if (hovered) {
			ImGui::GetWindowDrawList()->AddRectFilled(bgStart, itemEnd, IM_COL32(90, 90, 120, 100));
		}


		if (selected) {
			ImGui::GetWindowDrawList()->AddRectFilled(bgStart, itemEnd, IM_COL32(100, 100, 180, 120));
		}


		ImGui::InvisibleButton("##logline", ImVec2(itemEnd.x - bgStart.x, itemEnd.y - bgStart.y));
		if (ImGui::IsItemClicked()) {
			selectedIndex = selected ? -1 : i;
		}


		ImGui::SetCursorScreenPos(itemStart);
		ImGui::PushTextWrapPos(wrapX);

		if (line.warningState == LogMessage::MESSAGE) {
			ImGui::TextWrapped("%s", fullTxt.c_str());
		}
		else if (line.warningState == LogMessage::ERR) {
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", fullTxt.c_str());
		}
		else if (line.warningState == LogMessage::WARNING) {
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s", fullTxt.c_str());
		}

		if (copy && selected)
		{
			clipboard += line.message;
		}

		ImGui::PopTextWrapPos();
		ImGui::Dummy(ImVec2(0.0f, 0.0f)); // evtl. Platzhalter für Klickfläche
		ImGui::PopID();
	}


	bool OpenInVisualStudio2022(const std::string& fullFilePath, int line) {
		// Path zur VS 2022 Community (ggf. an Deine Edition anpassen)
		constexpr auto VS_PATH =
			R"(C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe)";

		// 1) /Edit öffnet die Datei
		std::string params = "/Edit \"" + fullFilePath + "\"";

		// 2) /Command führt den GoTo-Befehl innerhalb von VS aus
		params += " /Command \"Edit.GoTo " + std::to_string(line) + "\"";

		HINSTANCE result = ShellExecuteA(
			nullptr,              // Parent-HWND
			"open",               // Default verb
			VS_PATH,              // Programm
			params.c_str(),       // Parameter
			nullptr,              // Working directory
			SW_SHOWNORMAL         // Show window normally
		);

		return reinterpret_cast<intptr_t>(result) > 32;
	}

	void DebugWindow::draw() {
		if (!m_visible)
			return;

		ImGuiStyle& style = ImGui::GetStyle();
		style.ItemSpacing.y = 0;

		// Position and size
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImVec2 workPos = vp->WorkPos;
		ImVec2 workSize = vp->WorkSize;
		static float lastWidth = workSize.x * 0.2f;

		ImGui::SetNextWindowPos(workPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(lastWidth, workSize.y), ImGuiCond_Always);
		ImGui::SetNextWindowSizeConstraints(ImVec2(150, 100), ImVec2(workSize.x, workSize.y));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
		ImGui::Begin("Debug Window", nullptr, flags);
		lastWidth = ImGui::GetWindowSize().x;

		// Controls
		if (ImGui::Button("Clear")) {
			m_logs.clear();
			selectedIndex = -1;

			m_errorLogsCount = 0;
			m_warningLogsCount = 0;
			m_messageLogsCount = 0;
		}
		ImGui::SameLine();

		const ImVec4 enabledColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		const ImVec4 enabledHovered = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		const ImVec4 enabledActive = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

		const ImVec4 disabledColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);         // etwas matter
		const ImVec4 disabledHovered = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);
		const ImVec4 disabledActive = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive);

		static bool messageLogsEnabled = true;
		std::string msgLogs = "Logs";

		ImGui::PushStyleColor(ImGuiCol_Button, messageLogsEnabled ? enabledColor : disabledColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, messageLogsEnabled ? enabledHovered : disabledHovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, messageLogsEnabled ? enabledActive : disabledActive);


		if (ImGui::Button(msgLogs.c_str())) {
			messageLogsEnabled = !messageLogsEnabled;
		}


		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::Text(std::to_string(m_messageLogsCount).c_str());

		ImGui::SameLine();

		static bool warningLogsEnabled = true;
		std::string warnLogs = "Warnings";

		ImGui::PushStyleColor(ImGuiCol_Button, warningLogsEnabled ? enabledColor : disabledColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, warningLogsEnabled ? enabledHovered : disabledHovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, warningLogsEnabled ? enabledActive : disabledActive);

		if (ImGui::Button(warnLogs.c_str())) {
			warningLogsEnabled = !warningLogsEnabled;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::Text(std::to_string(m_warningLogsCount).c_str());

		ImGui::SameLine();

		// State
		static bool errorLogsEnabled = true;

		std::string errLabel = "Errors";

		ImGui::PushStyleColor(ImGuiCol_Button, errorLogsEnabled ? enabledColor : disabledColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, errorLogsEnabled ? enabledHovered : disabledHovered);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, errorLogsEnabled ? enabledActive : disabledActive);
		if (ImGui::Button(errLabel.c_str())) {
			errorLogsEnabled = !errorLogsEnabled;
		}
		ImGui::PopStyleColor(3);


		ImGui::SameLine();

		ImGui::Text(std::to_string(m_errorLogsCount).c_str());

		ImGui::SameLine();
		ImGui::SetNextItemWidth(125.0f);

		static char buf[256] = "";
		ImGui::InputText("Search", buf, sizeof(buf));

		ImGui::SameLine();

		ImGui::SetNextItemWidth(125.0f);
		ImGui::SliderFloat("Spacing", &m_itemSpacingY, 0.0f, 20.0f, "%.1f px");

		ImGui::Separator();

		// Main log area, reserve bottom space
		float selHeight = 100 + style.WindowPadding.y * 2;

		ImGui::BeginChild("LogRegion", ImVec2(0, -selHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
		bool wasAtBottom = ImGui::GetScrollY() + std::min<float>(ImGui::GetScrollMaxY() * 0.1f,25) >= ImGui::GetScrollMaxY();

		// Copy on Ctrl+C
		bool copyMain = ImGui::IsWindowFocused() && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C);
		std::string copyBuffer;

		style.ItemSpacing.y = m_itemSpacingY;

		if (buf[0] != '\0') {
			int realIndex = 0;

			for (int i = 0; i < (int)m_logs.size(); ++i) {
				if (m_logs[i].message.find(buf) != std::string::npos)
				{
					const uint8_t warning = m_logs[i].warningState;
					if (warning == LogMessage::MESSAGE && messageLogsEnabled || warning == LogMessage::ERR && errorLogsEnabled || warning == LogMessage::WARNING && warningLogsEnabled)
						DrawLogLine(i, realIndex++,m_itemSpacingY, copyMain, copyBuffer);
				}
			}
		}
		else {
			int realIndex = 0;

			for (int i = 0; i < (int)m_logs.size(); ++i) {
				const uint8_t warning = m_logs[i].warningState;

				if (warning == LogMessage::MESSAGE && messageLogsEnabled  || warning == LogMessage::ERR && errorLogsEnabled || warning == LogMessage::WARNING && warningLogsEnabled)
					DrawLogLine(i, realIndex++,m_itemSpacingY, copyMain, copyBuffer);
			}
		}

		style.ItemSpacing.y = 2.f;
		if (wasAtBottom)
			ImGui::SetScrollHereY(1.0f);

		if (copyMain && !copyBuffer.empty()) ImGui::SetClipboardText(copyBuffer.c_str());
		ImGui::EndChild();

		ImGui::Separator();

		const char* label = "Selected Message:";
		ImVec2 textSize = ImGui::CalcTextSize(label);
		float selectedSize = selHeight - textSize.y - (style.ItemSpacing.y * 3);
		ImGui::Text(label);


		ImGui::BeginChild("SelectedLog", ImVec2(0, selectedSize), true);
		ImGui::BeginDisabled(selectedIndex == -1);
		if (ImGui::Button("Copy Message")) {
			ImGui::SetClipboardText(m_logs[selectedIndex].message.c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("Copy Source Path")) {
			ImGui::SetClipboardText(m_logs[selectedIndex].sourcePath.c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("Copy All")) {
			ImGui::SetClipboardText(m_logs[selectedIndex].selectedMessage().c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("Open Path")) {
			OpenInVisualStudio2022(m_logs[selectedIndex].sourcePath, m_logs[selectedIndex].sourceLine);
		}

		ImGui::EndDisabled();

		ImGui::Separator();

		if (selectedIndex >= 0 && selectedIndex < (int)m_logs.size()) {
			LogMessage log = m_logs[selectedIndex];
			const std::string message = log.selectedMessage();

			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));

			ImGui::PushTextWrapPos(0.0f); // ← WRAP aktivieren
			ImGui::TextUnformatted(message.c_str());
			ImGui::PopTextWrapPos();

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();
	}
}