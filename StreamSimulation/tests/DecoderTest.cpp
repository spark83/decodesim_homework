#include <gtest/gtest.h>
#include <Decoder.hpp>

TEST(DecoderTest, DemoDecodeTest) {
    StreamSim::Core::DemoDecoder decoder;
    StreamSim::Core::ByteUndecodedFrame undecodedFrame;
    undecodedFrame.data = 4;
    StreamSim::Core::ByteFrameElement decodedFrame;
    decoder.DecodeFrameData(undecodedFrame, decodedFrame);
    EXPECT_EQ(decodedFrame.data, 2);

    undecodedFrame.data = 3;
    decoder.DecodeFrameData(undecodedFrame, decodedFrame);
    EXPECT_EQ(decodedFrame.data, 1);
}

TEST(DecoderTest, FrameElementDecodeServiceTest) {
    StreamSim::Core::AsyncByteFrameQueue decodeQueue;
    StreamSim::Core::AsyncByteFrameQueue renderQueue;
    StreamSim::Core::FrameElementQueueDecodeService decodeService(&decodeQueue, &renderQueue);
    StreamSim::Core::ByteUndecodedFrame undecodedFrame;
    undecodedFrame.data = 4;
    decodeQueue.WriteSync(undecodedFrame);
    undecodedFrame.data = 6;
    decodeQueue.WriteSync(undecodedFrame);
    undecodedFrame.data = 8;
    decodeQueue.WriteSync(undecodedFrame);
    undecodedFrame.data = 10;
    decodeQueue.WriteSync(undecodedFrame);

    decodeService.Run();
    decodeService.Shutdown();

    EXPECT_TRUE(decodeQueue.IsEmpty());
    EXPECT_TRUE(renderQueue.NumElements() == 4);

    StreamSim::Core::ByteFrameElement decodedFrame;
    EXPECT_TRUE(renderQueue.ReadSync(decodedFrame));
    EXPECT_EQ(decodedFrame.data, 2);
    EXPECT_TRUE(renderQueue.ReadSync(decodedFrame));
    EXPECT_EQ(decodedFrame.data, 3);
    EXPECT_TRUE(renderQueue.ReadSync(decodedFrame));
    EXPECT_EQ(decodedFrame.data, 4);
    EXPECT_TRUE(renderQueue.ReadSync(decodedFrame));
    EXPECT_EQ(decodedFrame.data, 5);
}