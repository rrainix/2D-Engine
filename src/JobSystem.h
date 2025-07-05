#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency())
        : stopFlag(false)
    {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back([this] { this->workerLoop(); });
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    // 1) Einfachen Task enqueuen, bekommt eine future<void>
    template<typename F>
    std::future<void> schedule(F&& fn) {
        return scheduleImpl(std::forward<F>(fn), nullptr);
    }

    // 2) Task mit Callback (wird nach fn() im Worker-Thread aufgerufen)
    template<typename F, typename C>
    std::future<void> schedule(F&& fn, C&& callback) {
        return scheduleImpl(std::forward<F>(fn),
            std::forward<C>(callback));
    }

    // 3) Shutdown: keine neuen Tasks, alle noch offenen abarbeiten
    void shutdown() {
        {
            std::unique_lock lock(mutex);
            stopFlag = true;
        }
        cv.notify_all();
        for (auto& w : workers) {
            if (w.joinable())
                w.join();
        }
        workers.clear();
    }

private:
    struct Task {
        std::function<void()> fn;    // main job
        std::function<void()> cb;    // optional callback
        std::promise<void> promise;  // zum Signalisieren der Fertigstellung
    };

    std::vector<std::thread> workers;
    std::queue<std::shared_ptr<Task>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    bool stopFlag;

    // Worker-Thread loop
    void workerLoop() {
        while (true) {
            std::shared_ptr<Task> task;
            {
                std::unique_lock lock(mutex);
                cv.wait(lock, [&] { return stopFlag || !tasks.empty(); });
                if (stopFlag && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }

            // 1. Job ausführen
            task->fn();
            // 2. Callback (falls vorhanden)
            if (task->cb) task->cb();
            // 3. Fertig melden
            task->promise.set_value();
        }
    }

    // Intern: enqueuet Aufgabe und liefert future
    template<typename F, typename C>
    std::future<void> scheduleImpl(F&& fn, C&& callback) {
        auto task = std::make_shared<Task>();
        task->fn = std::forward<F>(fn);

        if constexpr (!std::is_null_pointer_v<std::decay_t<C>>) {
            task->cb = std::forward<C>(callback);
        }

        auto fut = task->promise.get_future();
        {
            std::unique_lock lock(mutex);
            if (stopFlag)
                throw std::runtime_error("ThreadPool has been shut down");
            tasks.push(task);
        }
        cv.notify_one();
        return fut;
    }
};
