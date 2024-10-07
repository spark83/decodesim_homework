#include <gtest/gtest.h>
#include <StreamRenderer.hpp>

TEST(ProtocolServiceTest, DemoProtocolServiceTest) {
    testing::internal::CaptureStdout();
    StreamSim::Core::AsyncByteFrameQueue bufferQueue;
    StreamSim::Render::FrameElementRenderHandler renderHandler(&bufferQueue);

    StreamSim::Core::ByteFrameElement frame;
    renderHandler.Run();

    frame.data = 'a';
    bufferQueue.WriteSync(frame);
    frame.data = 'b';
    bufferQueue.WriteSync(frame);
    frame.data = 'c';
    bufferQueue.WriteSync(frame);
    frame.data = 'd';
    bufferQueue.WriteSync(frame);

    renderHandler.Shutdown();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ("a\nb\nc\nd\n", output);
}