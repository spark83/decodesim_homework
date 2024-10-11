#include <gtest/gtest.h>
#include <ProtocolService.hpp>

TEST(ProtocolServiceTest, DemoQueuedProtocolServiceTest) {
    StreamSim::Net::DemoProtocolServiceQueued service(4, 1);
    service.Run();
    service.Shutdown();

    EXPECT_EQ(service.GetNumDecodeBufferElements(), 0);
    EXPECT_EQ(service.GetNumDecodedBufferElements(), 0);
}

TEST(ProtocolServiceTest, DemoPooledProtocolServiceTest) {
    StreamSim::Net::DemoProtocolServicePooled service(4, 1);
    service.Run();
    service.Shutdown();

    EXPECT_EQ(service.GetNumDecodedBufferElements(), 0);
}
