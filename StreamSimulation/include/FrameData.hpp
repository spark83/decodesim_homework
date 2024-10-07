#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "ConcurrentData.hpp"

namespace StreamSim::Core {

constexpr uint32_t DEFULT_FRAME_BUFFER_SIZE = 1000;

template <typename T>
struct FrameElement {
    T data;
};

// In real-world scenario this will never be a byte but we're just assuming data contained in this
// has all necessary data to decode into an image.
using ByteUndecodedFrame = FrameElement<uint8_t>;
using ByteFrameElement = FrameElement<uint8_t>;
using AsyncByteFrameQueue = ConcurrentBufferQueue<ByteFrameElement, DEFULT_FRAME_BUFFER_SIZE>;

}
