#include <gtest/gtest.h>
#include <NetInputStream.hpp>

TEST(NetInputStreamTest, DemoNetInputStreamHandlerTest) {
    StreamSim::Core::AsyncByteFrameQueue bufferQueue;
    StreamSim::Net::DemoNetInputStreamHandler handler(&bufferQueue);
    StreamSim::Core::ByteFrameElement data;
    data.data = 2;
    handler.OnInputStreamData(data);
    data.data = 4;
    handler.OnInputStreamData(data);
    data.data = 6;
    handler.OnInputStreamData(data);
    data.data = 8;
    handler.OnInputStreamData(data);
    data.data = 10;
    handler.OnInputStreamData(data);

    EXPECT_EQ(bufferQueue.NumElements(), 5);

    EXPECT_TRUE(bufferQueue.ReadSync(data));
    EXPECT_EQ(data.data, static_cast<uint8_t>(2));
    EXPECT_TRUE(bufferQueue.ReadSync(data));
    EXPECT_EQ(data.data, static_cast<uint8_t>(4));
    EXPECT_TRUE(bufferQueue.ReadSync(data));
    EXPECT_EQ(data.data, static_cast<uint8_t>(6));
    EXPECT_TRUE(bufferQueue.ReadSync(data));
    EXPECT_EQ(data.data, static_cast<uint8_t>(8));
    EXPECT_TRUE(bufferQueue.ReadSync(data));
    EXPECT_EQ(data.data, static_cast<uint8_t>(10));
}
