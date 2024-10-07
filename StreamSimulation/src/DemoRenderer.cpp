#include <iostream>
#include "StreamRenderer.hpp"

namespace StreamSim::Render {
    FrameElementRenderHandler::FrameElementRenderHandler(Core::AsyncByteFrameQueue* frameReadBuffer)
    : m_readBuffer(frameReadBuffer)
    , m_isRunning(false) {
        assert(m_readBuffer != nullptr);
    }   

    FrameElementRenderHandler::~FrameElementRenderHandler() {
        Shutdown();
    }

    void FrameElementRenderHandler::PrintByteFrameElement(const StreamSim::Core::ByteFrameElement& frame) {
        std::lock_guard<std::mutex> lock(m_printMtx);

        std::cout << frame.data << std::endl;
    }

    void FrameElementRenderHandler::Run() {
        m_isRunning = true;
        m_renderThread = std::thread([this] {
            while (m_isRunning) {
                Core::ByteFrameElement data;
                if (!m_readBuffer->ReadSync(data)) {
                    continue;
                }
                PrintByteFrameElement(data);
            }
        });
    }

    void FrameElementRenderHandler::Shutdown() {
        m_isRunning = false;

        if (!m_renderThread.joinable()) {
            return;
        }

        m_renderThread.join();
    }
}