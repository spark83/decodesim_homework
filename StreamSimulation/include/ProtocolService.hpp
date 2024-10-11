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

class DemoProtocolServiceQueued : public ProtocolService {
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
    Core::FrameElementQueueDecodeService m_decodeService;

    // Rendering service.
    Render::FrameElementRenderHandler m_renderer;
    
public:
    DemoProtocolServiceQueued(std::size_t numThreads, uint32_t runTimeSec);
    ~DemoProtocolServiceQueued() override;

    bool Run() override;
    bool Shutdown() override;

    std::size_t GetNumDecodeBufferElements() const {
        return m_decodableBuffer->NumElements();
    }

    std::size_t GetNumDecodedBufferElements() const {
        return m_decodedBuffer->NumElements();
    }

    DemoProtocolServiceQueued(const DemoProtocolServiceQueued&) = delete;
};

class DemoProtocolServicePooled : public ProtocolService {
private:
    // This buffer data is created once and will be reused throughout the lifetime of the application
    std::unique_ptr<Core::AsyncByteFrameQueue> m_decodedBuffer;

    // Simulated thread with incoming streaming data which gets pushed into decodable buffer.
    std::size_t m_numIncomingDataThreads;
    uint32_t m_threadRunTime;
    std::vector<std::thread> m_incomingDataThreads;

    Core::FrameElementPoolDecoder m_poolDecoder;

    // Rendering service.
    Render::FrameElementRenderHandler m_renderer;

public:
    DemoProtocolServicePooled(std::size_t numThreads, uint32_t runTimeSec);
    ~DemoProtocolServicePooled() override;

    bool Run() override;
    bool Shutdown() override;

    std::size_t GetNumDecodedBufferElements() const {
        return m_decodedBuffer->NumElements();
    }

    DemoProtocolServicePooled(const DemoProtocolServicePooled&) = delete;
};

}
