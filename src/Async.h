#pragma once

#include <thread>
#include <chrono>
#include <utility>
#include <iostream>

namespace Async {
    // Internale Start-Funktion: führt den Funktionsobjekt in einem detached Thread aus
    template<typename Func>
    inline void start(Func&& func) {
        std::thread([f = std::forward<Func>(func)]() mutable {
            try {
                f();
            }
            catch (const std::exception& ex) {
                std::cerr << "[Async] Unhandled exception: " << ex.what() << std::endl;
            }
            catch (...) {
                std::cerr << "[Async] Unhandled unknown exception" << std::endl;
            }
            }).detach();
    }

    inline void WaitForSeconds(double seconds) {
        double timeScale = Time::timeScale();
        if (timeScale <= 0.0001) {
            // Zeit steht still, z.B. Pause – warte unendlich oder handle anders
            while (Time::timeScale() <= 0.0001) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        double scaledSeconds = seconds / timeScale;
        std::this_thread::sleep_for(std::chrono::duration<double>(scaledSeconds));
    }

    inline void waitForSecondsRealtime(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }

    // Warte asynchron (Thread wird blockiert) für die angegebene Anzahl Millisekunden
    inline void waitForMillisecondsRealtime(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    inline void waitForMilliseconds(int milliseconds) {
        double timeScale = Time::timeScale();
        if (timeScale <= 0.0001) {
            // Zeit steht still (Pause) – hier blockieren wir bis timeScale wieder > 0
            while (Time::timeScale() <= 0.0001) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            timeScale = Time::timeScale(); // neu holen, falls sich geändert
        }

        double scaledMs = milliseconds / timeScale;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(scaledMs + 0.5)));
    }
}

// Makro zum einfachen Aufruf: ASYNC(<Ausdruck>);
// Beispiel: ASYNC(Function()); oder ASYNC({ doA(); doB(); });
#define ASYNC(...) Async::start([=]() { __VA_ARGS__; })