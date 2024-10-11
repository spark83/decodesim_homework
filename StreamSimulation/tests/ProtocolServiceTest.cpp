#include <gtest/gtest.h>
#include <ProtocolService.hpp>

TEST(ProtocolServiceTest, DemoProtocolServiceTest) {
    StreamSim::Net::DemoProtocolServiceQueued service(4, 1);
    service.Run();
    service.Shutdown();

    EXPECT_EQ(service.GetNumDecodeBufferElements(), 0);
    EXPECT_EQ(service.GetNumDecodedBufferElements(), 0);
}
