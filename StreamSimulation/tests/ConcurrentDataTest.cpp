#include <gtest/gtest.h>
#include "ConcurrentData.hpp"

TEST(ConcurrentDataBufferTest, SingleThreadedWriteRead) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int value;

    buffer.WriteSync(10);
    EXPECT_EQ(buffer.NumElements(), 1);

    bool result = buffer.ReadSync(value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(buffer.NumElements(), 0);
}

TEST(ConcurrentDataBufferTest, SingleThread_WriteReadSync) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int data;

    // Write data to the buffer
    buffer.WriteSync(42);
    EXPECT_EQ(buffer.NumElements(), 1);

    // Read data from the buffer
    bool result = buffer.ReadSync(data);
    EXPECT_TRUE(result);
    EXPECT_EQ(data, 42);
    EXPECT_EQ(buffer.NumElements(), 0);
}

TEST(ConcurrentDataBufferTest, BufferFullCondition) {
    StreamSim::Core::ConcurrentBufferQueue<int, 2> buffer;

    buffer.WriteSync(1);
    buffer.WriteSync(2);

    std::thread writer([&buffer]() {
        buffer.WriteSync(3); // This should block until a read occurs
    });

    // Sleep for a short duration to ensure writer thread attempts to write
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(buffer.NumElements(), 2);

    int value;
    bool result = buffer.ReadSync(value);
    EXPECT_TRUE(result);
    EXPECT_EQ(value, 1);

    writer.join();

    EXPECT_EQ(buffer.NumElements(), 2);
}

TEST(ConcurrentDataBufferTest, SingleThread_ReadAsyncWhenEmpty) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int data;

    // Attempt to read from an empty buffer using ReadAsync
    bool result = buffer.ReadAsync(data);
    EXPECT_FALSE(result);
}

TEST(ConcurrentDataBufferTest, SingleThread_ReadAsyncWhenDataAvailable) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int data;

    // Write data to the buffer
    buffer.WriteSync(100);

    // Attempt to read using ReadAsync
    bool result = buffer.ReadAsync(data);
    EXPECT_TRUE(result);
    EXPECT_EQ(data, 100);
}

TEST(ConcurrentDataBufferTest, ConcurrentWriteRead) {
    StreamSim::Core::ConcurrentBufferQueue<int, 1000> buffer;

    const int num_items = 100;
    std::vector<int> written_values;
    std::vector<int> read_values;
    std::mutex vec_mutex;

    // Writer thread
    std::thread writer([&buffer, &written_values, &vec_mutex]() {
        for (int i = 0; i < num_items; ++i) {
            buffer.WriteSync(i);
            written_values.push_back(i);
        }
    });

    // Reader thread
    std::thread reader([&buffer, &read_values, &vec_mutex]() {
        int value;
        while (!buffer.IsEmpty()) {
            buffer.ReadSync(value);
            read_values.push_back(value);
        }
    });

    writer.join();
    reader.join();

    // Sort vectors to compare regardless of order
    std::sort(written_values.begin(), written_values.end());
    std::sort(read_values.begin(), read_values.end());

    EXPECT_EQ(written_values.size(), read_values.size());
    EXPECT_EQ(written_values, read_values);
}

TEST(ConcurrentDataBufferTest, MultipleReadersWriters) {
    StreamSim::Core::ConcurrentBufferQueue<int, 100> buffer;

    const int num_writers = 3;
    const int num_readers = 3;
    const int items_per_writer = 50;
    std::atomic<int> total_written{0};
    std::atomic<int> total_read{0};

    // Writer threads
    std::vector<std::thread> writers;
    for (int i = 0; i < num_writers; ++i) {
        writers.emplace_back([&buffer, &total_written, items_per_writer, i]() {
            for (int j = 0; j < items_per_writer; ++j) {
                buffer.WriteSync(i * items_per_writer + j);
                ++total_written;
            }
        });
    }

    // Reader threads
    std::vector<std::thread> readers;
    for (int i = 0; i < num_readers; ++i) {
        readers.emplace_back([&buffer, &total_read]() {
            int value;
            while (!buffer.IsEmpty()) {
                buffer.ReadSync(value);
                ++total_read;
            }
        });
    }

    // Wait for writers to finish
    for (auto& writer : writers) {
        writer.join();
    }

    // Wait for readers to finish
    for (auto& reader : readers) {
        reader.join();
    }

    EXPECT_EQ(total_written.load(), num_writers * items_per_writer);
    EXPECT_EQ(total_read.load(), total_written.load());
}

TEST(ConcurrentDataBufferTest, MultiThread_ReadBlocksWhenEmpty) {
    StreamSim::Core::ConcurrentBufferQueue<int, 2> buffer;
    std::atomic<bool> readBlocked{false};
    int data;

    // Reader thread attempts to read and should block
    std::thread readerThread([&buffer, &readBlocked, &data]() {
        readBlocked = true;
        bool result = buffer.ReadSync(data);
        readBlocked = false;
        EXPECT_TRUE(result);
    });

    // Allow time for the reader to block
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(readBlocked.load()); // Reader should be blocked

    // Write an item to unblock the reader
    buffer.WriteSync(99);

    readerThread.join();

    EXPECT_FALSE(readBlocked.load());
    EXPECT_EQ(data, 99);
    EXPECT_EQ(buffer.NumElements(), 0);
}
