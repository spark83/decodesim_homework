#pragma once

#include <memory>
#include <vector>
#include "NetInputStream.hpp"
#include "Decoder.hpp"
#include "StreamRenderer.hpp"

namespace StreamSim::Net {

class ProtocolService {
public:
    ProtocolService() = default;
    virtual ~ProtocolService() = default;

    // Simulate the service to run.
    virtual bool Run() = 0;

    // Shutdown the service.
    virtual bool Shutdown() = 0;
};

class DemoProtocolService : public ProtocolService {
private:
    // This buffer data is created once and will be reused throughout the lifetime of the application
    std::unique_ptr<Core::AsyncByteFrameQueue> m_decodableBuffer;
    std::unique_ptr<Core::AsyncByteFrameQueue> m_decodedBuffer;

    // Simulated thread with incoming streaming data which gets pushed into decodable buffer.
    std::size_t m_numIncomingDataThreads;
    uint32_t m_threadRunTime;
    std::vector<std::thread> m_incomingDataThreads;

    // Incoming data handler.
    DemoNetInputStreamHandler m_inputStreamHandler;
    
    // Decode service.
    Core::FrameElementDecodeService m_decodeService;

    // Rendering service.
    Render::FrameElementRenderHandler m_renderer;
    
public:
    DemoProtocolService(std::size_t numThreads, uint32_t runTimeSec);
    ~DemoProtocolService() override;

    bool Run() override;
    bool Shutdown() override;

    DemoProtocolService(const DemoProtocolService&) = delete;
};

}
