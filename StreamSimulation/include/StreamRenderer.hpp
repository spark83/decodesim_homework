#pragma once

#include <atomic>
#include "FrameData.hpp"

namespace StreamSim::Render {

// This class demonstrates how the decoded frame element data gets read and passed into
// imaginary renderer which handles all decoded video stream rendering.
// There class assume that there is a dedicated rendering thread that recevies frames from
// frame queue, somehow handles associating each frame to correct frame number and also to 
// correct user (in case of multiple users with video call), etc.
class FrameElementRenderHandler {
private:
    // This is non-owning raw pointer.
    Core::AsyncByteFrameQueue* m_readBuffer;
    std::thread m_renderThread;
    std::atomic_bool m_isRunning;
    
    // For debugging purpose.
    std::mutex m_printMtx;
    void PrintByteFrameElement(const StreamSim::Core::ByteFrameElement& frame);
    
public:
    FrameElementRenderHandler(Core::AsyncByteFrameQueue* frameReadBuffer);
    ~FrameElementRenderHandler();

    void Run();
    void Shutdown();
};

}
