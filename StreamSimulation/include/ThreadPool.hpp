#include <cstddef>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <concepts>

#pragma once

namespace StreamSim::Core {

// I am not gonna make this thread pool fancy by allowing mutiple parameters
// So for now it also allows a function that doesn't return anything and no function parameter.
template <typename Func>
concept Callable = requires(Func f) {
    { f() } -> std::same_as<void>;
};

// Super simplisitc thread pool class.
// This doesn't do anything fancy, it's only done this way to demonstrate how thread pool can be used to decode streaming video.
template <Callable Func, std::size_t N>
class SimpleThreadPool {
private:
    std::size_t m_numThreads;
    std::array<std::thread, N> m_workers;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::queue<Func> m_tasks;

    // If there are more worker tasks than allowed number of tasks, this gets queued up and gets added when it can be added.
    std::queue<Func> m_tasksToBeAdded;

    bool m_isRunning;

    void Worker() {
        while (true) {
            Func task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this] { return !m_tasks.empty() || !m_isRunning; });

                if (!m_isRunning && m_tasks.empty())
                    return;

                task = m_tasks.front();
                m_tasks.pop();

                while (!m_tasksToBeAdded.empty()) {
                    m_tasks.push(m_tasksToBeAdded.front());
                    m_tasksToBeAdded.pop();
                }
            }
            task();
        }
    }

public:
    SimpleThreadPool()
    : m_isRunning(true) {
        std::for_each(m_workers.begin(), m_workers.end(), [this] (std::thread& th) {
            th = std::thread([this]() { this->Worker(); });
        });
    }

    ~SimpleThreadPool() {
        Stop();
    }

    // Add a job to the queue
    void Enqueue(Func function) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_tasks.size() < N) {
                m_tasks.push(function);
            } else {
                m_tasksToBeAdded.push(function);
            }
        }

        m_cv.notify_one();
    }

    void Stop() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isRunning = false;
        }
        m_cv.notify_all();
        std::for_each(m_workers.begin(), m_workers.end(), [this] (std::thread& th) {
            if (th.joinable()) {
                th.join();
            }
        });
    }
};

}
