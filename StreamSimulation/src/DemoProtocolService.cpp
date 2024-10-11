#include <chrono>
#include <random>
#include "ProtocolService.hpp"

namespace {
    constexpr uint32_t DEFAULT_RECEIVE_DATA_FREQUENCY = 1;
}

namespace StreamSim::Net {

DemoProtocolServiceQueued::DemoProtocolServiceQueued(std::size_t numThreads, uint32_t runTimeSec)
: m_decodableBuffer(std::make_unique<Core::AsyncByteFrameQueue>())
, m_decodedBuffer(std::make_unique<Core::AsyncByteFrameQueue>())
, m_numIncomingDataThreads(numThreads)
, m_threadRunTime(runTimeSec)
, m_inputStreamHandler(m_decodableBuffer.get())
, m_decodeService(m_decodableBuffer.get(), m_decodedBuffer.get())
, m_renderer(m_decodedBuffer.get()) {}

DemoProtocolServiceQueued::~DemoProtocolServiceQueued() {
    Shutdown();
}

bool DemoProtocolServiceQueued::Run() {
    for (std::size_t i = 0; i < m_numIncomingDataThreads; ++i) {
        m_incomingDataThreads.emplace_back(std::thread([this] {
            // Initialize random number generator
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<int> distribution(0, 255);

            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + std::chrono::seconds(m_threadRunTime);

            int count = 0;
            
            while (std::chrono::high_resolution_clock::now() < end) {
                // Just generate random number between 0 - 255 for the sake of simulating
                // incoming streaming data.  Of course, this isn't really indicative of what real data is going to be like
                // but for this task, this should be enough.
                auto randNum = distribution(generator);
                Core::ByteUndecodedFrame data;
                data.data = static_cast<uint8_t>(randNum);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_RECEIVE_DATA_FREQUENCY));
                m_inputStreamHandler.OnInputStreamData(data);
            }
        }));
    }

    m_decodeService.Run();
    m_renderer.Run();

    return true;
}

bool DemoProtocolServiceQueued::Shutdown() {
    for_each(m_incomingDataThreads.begin(), m_incomingDataThreads.end(), [](std::thread& th) {
        if (th.joinable()) {
            th.join();
        }
    });

    m_decodeService.Shutdown();
    m_renderer.Shutdown();

    m_incomingDataThreads.clear();

    return true;
}

/*
    // Simulated thread with incoming streaming data which gets pushed into decodable buffer.
    std::size_t m_numIncomingDataThreads;
    uint32_t m_threadRunTime;
    std::vector<std::thread> m_incomingDataThreads;

    Core::FrameElementPoolDecoder m_poolDecoder;

    // Rendering service.
    Render::FrameElementRenderHandler m_renderer;
*/
DemoProtocolServicePooled::DemoProtocolServicePooled(std::size_t numThreads, uint32_t runTimeSec)
: m_decodedBuffer(std::make_unique<Core::AsyncByteFrameQueue>())
, m_numIncomingDataThreads(numThreads)
, m_threadRunTime(runTimeSec)
, m_poolDecoder(m_decodedBuffer.get())
, m_renderer(m_decodedBuffer.get()) {}

DemoProtocolServicePooled::~DemoProtocolServicePooled() {

}

bool DemoProtocolServicePooled::Run() {
    for (std::size_t i = 0; i < m_numIncomingDataThreads; ++i) {
        m_incomingDataThreads.emplace_back(std::thread([this] {
            // Initialize random number generator
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<int> distribution(0, 255);

            auto start = std::chrono::high_resolution_clock::now();
            auto end = start + std::chrono::seconds(m_threadRunTime);

            int count = 0;
            
            while (std::chrono::high_resolution_clock::now() < end) {
                // Just generate random number between 0 - 255 for the sake of simulating
                // incoming streaming data.  Of course, this isn't really indicative of what real data is going to be like
                // but for this task, this should be enough.
                auto randNum = distribution(generator);
                Core::ByteUndecodedFrame data;
                data.data = static_cast<uint8_t>(randNum);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_RECEIVE_DATA_FREQUENCY));
                m_poolDecoder.OnInputStreamData(data);
            }
        }));
    }

    m_renderer.Run();

    return true;
}

bool DemoProtocolServicePooled::Shutdown() {
    for_each(m_incomingDataThreads.begin(), m_incomingDataThreads.end(), [](std::thread& th) {
        if (th.joinable()) {
            th.join();
        }
    });

    m_renderer.Shutdown();

    m_incomingDataThreads.clear();

    return true;
}
}
