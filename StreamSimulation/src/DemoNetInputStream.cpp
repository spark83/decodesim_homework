#include <cassert>

#include "NetInputStream.hpp"

namespace StreamSim::Net {

DemoNetInputStreamHandler::DemoNetInputStreamHandler(Core::AsyncByteFrameQueue* buffer)
: m_buffer(buffer) {
    assert(m_buffer != nullptr);
}

DemoNetInputStreamHandler::~DemoNetInputStreamHandler() {}

void DemoNetInputStreamHandler::OnInputStreamData(const Core::ByteUndecodedFrame& data) {
    // Write to async buffer for this to be decoded later.
    m_buffer->WriteSync(data);
}

}
