#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <condition_variable>
#include <shared_mutex>
#include <cassert>
#include <chrono>

namespace StreamSim::Core {

// Buffer queue that has fixed size buffer which holds elements.
template <typename T, std::size_t N, uint32_t DefaultWaitSec = 2>
class ConcurrentBufferQueue {
private:
    std::array<T, N> m_dataBuffer;

    std::mutex m_mutex;
    std::condition_variable m_fullCv;
    std::condition_variable m_emptyCv;

    std::size_t m_count = 0;
    std::size_t m_head = 0;
    std::size_t m_tail = 0;

public:
    
    ConcurrentBufferQueue() = default;

    bool WriteSync(const T& data) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if constexpr (DefaultWaitSec == 0) {
            m_fullCv.wait(lock, [this] {
                return m_count < N;
            });
        } else {
            m_fullCv.wait_for(lock, std::chrono::seconds(DefaultWaitSec), [this] {
                return m_count < N;
            });
        }

        if (m_count >= N) {
            return false;
        }

        m_dataBuffer[m_tail++] = data;
        m_tail = m_tail % N;
        m_count++;
        
        m_emptyCv.notify_all();

        return true;
    }

    bool ReadAsync(T& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_count == 0) {
            return false;
        }

        data = m_dataBuffer[m_head++];
        m_head = m_head % N;
        m_count--;

        m_fullCv.notify_all();
        return true;
    }

    bool ReadSync(T& data) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if constexpr (DefaultWaitSec == 0) {
            m_emptyCv.wait(lock, [this] {
                return m_count > 0;
            });
        } else {
            m_emptyCv.wait_for(lock, std::chrono::seconds(DefaultWaitSec), [this] {
                return m_count > 0;
            });
        }

        if (m_count == 0) {
            return false;
        }

        data = m_dataBuffer[m_head++];
        m_head = m_head % N;
        m_count--;

        m_fullCv.notify_all();
        return true;
    }

    std::size_t GetHeadIndex() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_head;
    }

    std::size_t GetTailIndex() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tail;
    }

    std::size_t NumElements() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_count;
    }

    bool IsFull() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_count >= N;
    }

    bool IsEmpty() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_count == 0;
    }
};

}
