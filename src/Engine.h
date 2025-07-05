// Engine.h
class Engine {
public:
    static Engine& Get();
    EngineSettings settings;

    void LoadSettings(const std::string& path);
    void SaveSettings(const std::string& path);
    // …
};