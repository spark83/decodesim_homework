#include <cassert>
#include <chrono>
#include <iostream>
#include "Decoder.hpp"

namespace {
    constexpr uint32_t DEFAULT_TIME_TO_DECODE_IN_MILISEC = 4;
}

namespace StreamSim::Core {

DecoderTask::DecoderTask(const Core::ByteUndecodedFrame& frame,
                         Decoder* decoder,
                         Core::AsyncByteFrameQueue* renderBufferQueue)
: m_frame(frame)
, m_decoder(decoder)
, m_renderBufferQueue(renderBufferQueue) {
    assert(decoder != nullptr);
    assert(renderBufferQueue != nullptr);
}

DecoderTask::~DecoderTask() {}

void DecoderTask::operator()() {
    Core::ByteFrameElement decoded;
    m_decoder->DecodeFrameData(m_frame, decoded);
    m_renderBufferQueue->WriteSync(decoded);
}

DemoDecoder::DemoDecoder() {}

DemoDecoder::~DemoDecoder() {}

void DemoDecoder::DecodeFrameData(const Core::ByteUndecodedFrame& frame, Core::ByteFrameElement& decoded) {
    // Let's just assume decoding takes about 5 miliseconds, and decoding is just devide the frame value by 2.
    std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_TIME_TO_DECODE_IN_MILISEC));
    decoded.data = frame.data / 2;
}

FrameElementQueueDecodeService::FrameElementQueueDecodeService(Core::AsyncByteFrameQueue* decodeQueue, Core::AsyncByteFrameQueue* renderQueue)
: m_decodeBufferQueue(decodeQueue)
, m_renderBufferQueue(renderQueue)
, m_isRunning(false) {
    assert(m_decodeBufferQueue != nullptr);
    assert(m_renderBufferQueue != nullptr);
}

FrameElementQueueDecodeService::~FrameElementQueueDecodeService() {
    Shutdown();
}

void FrameElementQueueDecodeService::Run() {
    m_isRunning.store(true, std::memory_order_release);

    for (std::size_t i = 0; i < MAX_NUM_DECODER_THREADS; ++i) {
        m_decodeThreads[i] = std::thread([this] {
            Core::ByteUndecodedFrame undecodedFrame;
            Core::ByteFrameElement decodedFrame;

            while (m_isRunning.load(std::memory_order_acquire)) {
                if (m_decodeBufferQueue->IsEmpty()) {
                    continue;
                }
                m_decodeBufferQueue->ReadSync(undecodedFrame);
                m_mainDecoder.DecodeFrameData(undecodedFrame, decodedFrame);
                m_renderBufferQueue->WriteSync(decodedFrame);
            }

            while (!m_decodeBufferQueue->IsEmpty()) {
                m_decodeBufferQueue->ReadSync(undecodedFrame);
                m_mainDecoder.DecodeFrameData(undecodedFrame, decodedFrame);
                m_renderBufferQueue->WriteSync(decodedFrame);
            }
        });
    }
}

void FrameElementQueueDecodeService::Shutdown() {
    m_isRunning.store(false, std::memory_order_release);
    
    for_each(m_decodeThreads.begin(), m_decodeThreads.end(), [] (std::thread& th) {
        if (th.joinable()) {
            th.join();
        }
    });
}

FrameElementPoolDecoder::FrameElementPoolDecoder(Core::AsyncByteFrameQueue* renderQueue)
: m_renderBufferQueue(renderQueue) {}

FrameElementPoolDecoder::~FrameElementPoolDecoder() {}

void FrameElementPoolDecoder::OnInputStreamData(const Core::ByteUndecodedFrame& data) {
    m_decodePool.Enqueue(DecoderTask(data, &m_mainDecoder, m_renderBufferQueue));
}

}
