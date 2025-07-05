#include "Profiler.h"
#include <algorithm>
#include "Utils/Time.h"
#include "Graphics/Gizmos.h"

namespace engine {
	Profiler& Profiler::Get() {
		static Profiler instance;
		return instance;
	}

	Profiler::Profiler() {
		m_SecondTimer = std::chrono::high_resolution_clock::now();
		// Default tabs enabled
		m_TabEnabled["CPU"] = true;
		m_TabEnabled["GPU"] = true;
		m_TabEnabled["Memory"] = true;
		m_TabEnabled["General"] = true;
	}

	Profiler::~Profiler() {}

	void Profiler::BeginFrame() {
		// Called at frame start
	}

	void Profiler::EndFrame() {
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto now = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double>(now - m_SecondTimer).count();
		if (elapsed >= 1.0) {
			UpdateAggregates();
			ResetFrameData();
			m_SecondTimer = now;
		}
	}

	void Profiler::RecordCPU(CPUCategory category, double durationMs) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		std::string key;
		switch (category) {
		case CPUCategory::Rendering: key = "Rendering"; break;
		case CPUCategory::Physics:   key = "Physics";   break;
		case CPUCategory::Update:     key = "Update";     break;
		case CPUCategory::Fixedupdate:     key = "Fixedupdate";     break;
		}
		auto& entry = m_CPUStatEntries[key];
		entry.accumulated += durationMs;
		entry.samples++;
	}

	void Profiler::SetCPUStatistic(const std::string& name, const std::string& info) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_CPUStats[name] = info;
	}

	void Profiler::SetGPUStatistic(const std::string& name, const std::string& info) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_GPUStats[name] = info;
	}

	void Profiler::SetMemoryStatistic(const std::string& name, const std::string& info) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_MemoryStats[name] = info;
	}

	void Profiler::SetGeneralStatistic(const std::string& name, const std::string& info) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_GeneralStats[name] = info;
	}

	void Profiler::ToggleTab(const std::string& tabName, bool enabled) {
		m_TabEnabled[tabName] = enabled;
	}

	// Implementierung der Aggregations-Funktionen:
	void Profiler::UpdateAggregates() {
		// Berechne für jede Kategorie den Durchschnittswert (lastValue)
		for (auto& kv : m_CPUStatEntries) {
			StatEntry& e = kv.second;
			if (e.samples > 0) {
				e.lastValue = e.accumulated / e.samples;
			}
			else {
				e.lastValue = 0.0;
			}
		}
	}

	void Profiler::ResetFrameData() {
		// Setze Akkumulatoren und Zähler zurück für die nächste Sekunde
		for (auto& kv : m_CPUStatEntries) {
			kv.second.accumulated = 0.0;
			kv.second.samples = 0;
		}
	}
	void Profiler::Render() {
		ImGui::Begin("Profiler");
		// Tab toggles
		for (auto& kv : m_TabEnabled) {
			ImGui::Checkbox(kv.first.c_str(), &kv.second);
			ImGui::SameLine();
		}
		ImGui::NewLine();

		if (m_TabEnabled["CPU"]) {
			ImGui::PushID(0);

			if (ImGui::CollapsingHeader("CPU")) {
				auto interpColor = [&](float ms) {
					const float maxMs = 25.0f;
					float t = std::clamp(ms / maxMs, 0.0f, 1.0f);
					return ImVec4(
						/* R */ 1.0f,
						/* G */ 1.0f - t,
						/* B */ 1.0f - t,
						/* A */ 1.0f
					);
					};

				for (const auto& [name, e] : m_CPUStatEntries) {
					ImGui::PushStyleColor(ImGuiCol_Text, interpColor(static_cast<float>(e.lastValue)));
					ImGui::Text("%s: %.2f ms", name.c_str(), e.lastValue);
					ImGui::PopStyleColor();
				}

				for (const auto& kv : m_CPUStats) {
					std::string info = kv.first + ": " + kv.second;
					ImGui::Text(info.c_str());
				}
			}

			ImGui::PopID();
		}

		if (m_TabEnabled["GPU"]) {
			ImGui::PushID(1);

			if (ImGui::CollapsingHeader("GPU")) {
				for (const auto& kv : m_GPUStats) {
					std::string info = kv.first + ": " + kv.second;
					ImGui::Text(info.c_str());
				}
			}

			ImGui::PopID();
		}

		if (m_TabEnabled["Memory"]) {
			ImGui::PushID(2);

			if (ImGui::CollapsingHeader("Memory")) {
				for (const auto& kv : m_MemoryStats) {
					std::string info = kv.first + ": " + kv.second;
					ImGui::Text(info.c_str());
				}
			}

			ImGui::PopID();
		}

		if (m_TabEnabled["General"]) {
			ImGui::PushID(3);

			if (ImGui::CollapsingHeader("General")) {
				for (const auto& kv : m_GeneralStats) {
					std::string info = kv.first + ": " + kv.second;
					ImGui::Text(info.c_str());
				}
			}

			ImGui::PopID();
		}

		ImGui::End();
	}

	// ProfileScope implementation
	ProfileScope::ProfileScope(const char* name, Profiler::CPUCategory cat)
		: m_Name(name), m_Category(cat), m_Start(std::chrono::high_resolution_clock::now()) {
	}

	ProfileScope::~ProfileScope() {
		auto end = std::chrono::high_resolution_clock::now();
		double ms = std::chrono::duration<double, std::milli>(end - m_Start).count();
		Profiler::Get().RecordCPU(m_Category, ms);
	}

}
