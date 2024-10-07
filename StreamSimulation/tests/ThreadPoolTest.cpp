#include <gtest/gtest.h>
#include <atomic>
#include "ThreadPool.hpp" 

// A simple function to be used for testing
void TestTask(int& counter) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Simulate some work
    ++counter;
}

// Test that tasks are being processed correctly
TEST(SimpleThreadPoolTest, ProcessTasks) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 4> pool;

    int counter = 0;

    for (int i = 0; i < 4; ++i) {
        pool.Enqueue([&counter]() { TestTask(counter); });
    }

    // Give some time for tasks to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_EQ(counter, 4);  // All tasks should have been processed

    pool.Stop();  // Ensure pool shuts down properly
}

// Test that overflow tasks get processed once there is space
TEST(SimpleThreadPoolTest, OverflowQueue) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 2> pool;  // Only 2 threads

    int counter = 0;

    // Enqueue 4 tasks, 2 should go to the overflow queue
    for (int i = 0; i < 4; ++i) {
        pool.Enqueue([&counter]() { TestTask(counter); });
    }

    // Give some time for tasks to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_EQ(counter, 4);  // All tasks, including overflow, should be processed

    pool.Stop();  // Ensure pool shuts down properly
}

// Test that the pool stops correctly
TEST(SimpleThreadPoolTest, StopPool) {
    StreamSim::Core::SimpleThreadPool<std::function<void()>, 3> pool;

    int counter = 0;

    pool.Enqueue([&counter]() { TestTask(counter); });
    pool.Enqueue([&counter]() { TestTask(counter); });

    pool.Stop();  // Should stop properly without hanging

    EXPECT_EQ(counter, 2);  // Ensure that tasks were processed before stopping
}
