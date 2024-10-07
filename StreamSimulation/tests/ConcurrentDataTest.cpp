#include <gtest/gtest.h>
#include "ConcurrentData.hpp"

TEST(ConcurrentDataTest, SingleThreadedWriteRead) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int value;

    buffer.WriteSync(10);
    EXPECT_EQ(buffer.NumElements(), 1);

    EXPECT_TRUE(buffer.ReadSync(value));
    EXPECT_EQ(value, 10);
    EXPECT_EQ(buffer.NumElements(), 0);
}

TEST(ConcurrentDataTest, TryReadFromEmptyBufferWithTimerWait) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int value;

    // Should wait 2 second and fail.
    EXPECT_FALSE(buffer.ReadSync(value));
}

TEST(ConcurrentDataTest, TryReadFromEmptyBufferWithoutTimerWait) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int value;

    std::thread writer([&buffer]() {
        buffer.WriteSync(3);
        buffer.WriteSync(4);
        buffer.WriteSync(5);
    });

    EXPECT_TRUE(buffer.ReadSync(value));
    writer.join();

    EXPECT_EQ(buffer.NumElements(), 2);
}

TEST(ConcurrentDataTest, BufferFullCondition) {
    StreamSim::Core::ConcurrentBufferQueue<int, 2, 0> buffer;

    buffer.WriteSync(1);
    buffer.WriteSync(2);

    std::thread writer([&buffer]() {
        buffer.WriteSync(3);
    });

    // Sleep for a bit to make sure writer thread attempts to write
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(buffer.NumElements(), 2);

    int value;
    EXPECT_TRUE(buffer.ReadSync(value));
    EXPECT_EQ(value, 1);

    writer.join();

    EXPECT_EQ(buffer.NumElements(), 2);
}

TEST(ConcurrentDataTest, ReadAsyncWhenEmpty) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int data;

    EXPECT_FALSE(buffer.ReadAsync(data));
}

TEST(ConcurrentDataTest, ReadAsyncWhenDataAvailable) {
    StreamSim::Core::ConcurrentBufferQueue<int, 5> buffer;
    int data;

    // Write data to the buffer
    buffer.WriteSync(100);

    // Attempt to read using ReadAsync
    EXPECT_TRUE(buffer.ReadAsync(data));
    EXPECT_EQ(data, 100);
}

TEST(ConcurrentDataTest, ConcurrentWriteRead) {
    StreamSim::Core::ConcurrentBufferQueue<int, 1000> buffer;

    const int numItems = 100;
    std::vector<int> writeValues;
    std::vector<int> readValues;

    // Writer thread
    std::thread writer([&buffer, &writeValues]() {
        for (int i = 0; i < numItems; ++i) {
            buffer.WriteSync(i);
            writeValues.push_back(i);
        }
    });

    // Reader thread
    std::thread reader([&buffer, &readValues]() {
        int value;
        while (!buffer.IsEmpty()) {
            buffer.ReadSync(value);
            readValues.push_back(value);
        }
    });

    writer.join();
    reader.join();

    std::sort(writeValues.begin(), writeValues.end());
    std::sort(readValues.begin(), readValues.end());

    EXPECT_EQ(writeValues.size(), readValues.size());
    EXPECT_EQ(writeValues, readValues);
}

TEST(ConcurrentDataTest, MultipleReadersWriters) {
    StreamSim::Core::ConcurrentBufferQueue<int, 100> buffer;

    const int itemsPerWrite = 50;
    std::atomic<int> writeCount{0};
    std::atomic<int> readCount{0};

    // Writer threads
    std::vector<std::thread> writers;
    for (int i = 0; i < 3; ++i) {
        writers.emplace_back([&buffer, &writeCount, itemsPerWrite, i]() {
            for (int j = 0; j < itemsPerWrite; ++j) {
                buffer.WriteSync(i * itemsPerWrite + j);
                writeCount++;
            }
        });
    }

    // Reader threads
    std::vector<std::thread> readers;
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([&buffer, &readCount]() {
            while (!buffer.IsEmpty()) {
                int value;
                buffer.ReadSync(value);
                readCount++;
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

    EXPECT_EQ(writeCount.load(), 150);
    EXPECT_EQ(readCount.load(), writeCount.load());
}

TEST(ConcurrentDataTest, MultiThreadReadBlocksWhenEmpty) {
    StreamSim::Core::ConcurrentBufferQueue<int, 2> buffer;
    std::atomic<bool> readBlocked{false};
    int data;

    std::thread readerThread([&buffer, &readBlocked, &data]() {
        readBlocked = true;
        bool result = buffer.ReadSync(data);
        readBlocked = false;
        EXPECT_TRUE(result);
    });

    // Wait to ensure read function is blocking in another thread
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(readBlocked.load());

    // Write an item to unblock the reader
    buffer.WriteSync(99);

    readerThread.join();

    EXPECT_FALSE(readBlocked.load());
    EXPECT_EQ(data, 99);
    EXPECT_EQ(buffer.NumElements(), 0);
}
