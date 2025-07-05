#pragma once

#include <chrono>
#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include "imgui/imgui.h"
#include <glm/glm.hpp>

namespace engine {

    class Profiler {
    public:
        enum class CPUCategory { Rendering, Physics, Update, Fixedupdate };

        static Profiler& Get();
        void BeginFrame();
        void EndFrame();

        // Recording functions
        void RecordCPU(CPUCategory category, double durationMs);
        void SetCPUStatistic(const std::string& name, const std::string& info);
        void SetGPUStatistic(const std::string& name, const std::string& info);
        void SetMemoryStatistic(const std::string& name, const std::string& info);
        void SetGeneralStatistic(const std::string& name, const std::string& info);

        // UI toggles
        void ToggleTab(const std::string& tabName, bool enabled);

        // Render UI
        void Render();
        glm::vec4 color{ 1.f };
    private:
        Profiler();
        ~Profiler();

        void ResetFrameData();
        void UpdateAggregates();

        // Data structures
        struct StatEntry {
            double accumulated;
            int samples;
            double lastValue;
        };

        std::map<std::string, StatEntry> m_CPUStatEntries;
        std::map<std::string, std::string> m_CPUStats;

        std::map<std::string, std::string> m_GPUStats;
        std::map<std::string, std::string> m_MemoryStats;
        std::map<std::string, std::string> m_GeneralStats;

        std::unordered_map<std::string, bool> m_TabEnabled;

        std::chrono::high_resolution_clock::time_point m_SecondTimer;
        std::mutex m_Mutex;
    };



    // RAII scope helper
    struct ProfileScope {
        ProfileScope(const char* name, Profiler::CPUCategory cat);
        ~ProfileScope();

    private:
        const char* m_Name;
        Profiler::CPUCategory m_Category;
        std::chrono::high_resolution_clock::time_point m_Start;

    };

    // Profiling macros
#ifdef ENABLE_PROFILING
#define PROFILE_CPU(name, cat) ProfileScope scope##__LINE__(name, cat)
#define SET_CPU_STAT(name, info)     ::engine::Profiler::Get().SetCPUStatistic(name, info)
#define SET_GPU_STAT(name, info)     ::engine::Profiler::Get().SetGPUStatistic(name, info)
#define SET_MEM_STAT(name, info)     ::engine::Profiler::Get().SetMemoryStatistic(name, info)
#define SET_GEN_STAT(name, info)     ::engine::Profiler::Get().SetGeneralStatistic(name, info)
#else
#define PROFILE_CPU(name, cat)
#define SET_CPU_STAT(name, info)
#define SET_GPU_STAT(name, info)   
#define SET_MEM_STAT(name, info)
#define SET_GEN_STAT(name, info)
#endif
}