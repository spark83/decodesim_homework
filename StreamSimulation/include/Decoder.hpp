#pragma once

#include <thread>
#include <atomic>
#include "ConcurrentData.hpp"
#include "FrameData.hpp"
#include "ThreadPool.hpp"
#include "NetInputStream.hpp"

namespace StreamSim::Core {

constexpr size_t MAX_NUM_DECODER_THREADS = 4;

class Decoder {
public:
    Decoder() = default;
    virtual ~Decoder() = default;

    virtual void DecodeFrameData(const Core::ByteUndecodedFrame& frame, Core::ByteFrameElement& decoded) = 0;
};

class DecoderTask {
private:
    Core::ByteUndecodedFrame m_frame;
    Decoder* m_decoder;
    Core::AsyncByteFrameQueue* m_renderBufferQueue;

public:
    DecoderTask(const Core::ByteUndecodedFrame& frame,
                Decoder* decoder,
                Core::AsyncByteFrameQueue* renderBufferQueue);
    ~DecoderTask();

    void operator()();
};

// Upon doing some research, when it comes to decoding streaming video, there is an I, P, and B frame types
// And you need to use these frame types to decode most recent frame.  (Although B frame requires future frame
// according to my research?)  
// Well... This demo decoder doesn't know that there is such frame types exists and doesn't really do any decoding
// Again, this code is just to demonstrate how I would go about setting up the architecture and how efficently use
// the thread to ensure fastest decoding and fastest rendering of decoded data.
class DemoDecoder : public Decoder {
public:
    DemoDecoder();
    ~DemoDecoder() override;

    void DecodeFrameData(const Core::ByteUndecodedFrame& frame, Core::ByteFrameElement& decoded) override;
};

// All decoding tasks will be handled by this service class.
// There will be multiple threads that are available and performs the task
// In a real world scenario, there should be a thread pool that can be used for
// decoding task, but for this task, multiple thread that are already created will be used.
class FrameElementQueueDecodeService {
private:
    std::array<std::thread, MAX_NUM_DECODER_THREADS> m_decodeThreads;

    // This is non-owning raw pointer.
    Core::AsyncByteFrameQueue* m_decodeBufferQueue;
    Core::AsyncByteFrameQueue* m_renderBufferQueue;
    std::atomic_bool m_isRunning;

    DemoDecoder m_mainDecoder;

public:
    FrameElementQueueDecodeService(Core::AsyncByteFrameQueue* decodeQueue, Core::AsyncByteFrameQueue* renderQueue);
    ~FrameElementQueueDecodeService();

    void Run();
    void Shutdown();
};

class FrameElementPoolDecoder : public Net::NetInputStreamHandler {
private:
    Core::SimpleThreadPool<DecoderTask, MAX_NUM_DECODER_THREADS> m_decodePool;
    Core::AsyncByteFrameQueue* m_renderBufferQueue;
    DemoDecoder m_mainDecoder;

public:
    FrameElementPoolDecoder(Core::AsyncByteFrameQueue* renderQueue);
    ~FrameElementPoolDecoder();

    void OnInputStreamData(const Core::ByteUndecodedFrame& data) override;
};

}
