#include <gtest/gtest.h>
#include <atomic>
#include "ThreadPool.hpp" 

void SomeWorkTask(int& counter) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ++counter;
}

TEST(SimpleThreadPoolTest, ProcessTasks) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 4> pool;

    int counter = 0;

    for (int i = 0; i < 4; ++i) {
        pool.Enqueue([&counter]() { SomeWorkTask(counter); });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_EQ(counter, 4);

    pool.Stop();
}

TEST(SimpleThreadPoolTest, OverflowQueue) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 2> pool;

    int counter = 0;

    for (int i = 0; i < 4; ++i) {
        pool.Enqueue([&counter]() { SomeWorkTask(counter); });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(counter, 4);

    pool.Stop();
}

TEST(SimpleThreadPoolTest, StopPool) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 3> pool;

    int counter = 0;

    pool.Enqueue([&counter]() { SomeWorkTask(counter); });
    pool.Enqueue([&counter]() { SomeWorkTask(counter); });

    pool.Stop();

    EXPECT_EQ(counter, 2);
}
