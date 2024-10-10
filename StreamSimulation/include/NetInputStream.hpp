#pragma once

#include "FrameData.hpp"

namespace StreamSim::Net {

// Note: This is not a complete class, this class is created to assume these functions will be called in whatever real-time
// transport protocol will call these functions.
class NetInputStreamHandler {
public:
    virtual ~NetInputStreamHandler() = default;
    virtual void OnInputStreamData(const Core::ByteUndecodedFrame& data) = 0;
};

class DemoNetInputStreamHandler : public NetInputStreamHandler {
private:
    // This is non-owning raw pointer.
    Core::AsyncByteFrameQueue* m_buffer;

public:
    DemoNetInputStreamHandler(Core::AsyncByteFrameQueue* buffer);
    ~DemoNetInputStreamHandler() override;

    // Note: This function will be used as a callback function when the data is received from
    //       the network.  
    void OnInputStreamData(const Core::ByteUndecodedFrame& data) override;
};

}
