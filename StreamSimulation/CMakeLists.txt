set(STREAMSIM_HEADER_FILES
    "include/ConcurrentData.hpp"
    "include/Decoder.hpp"
    "include/FrameData.hpp"
    "include/NetInputStream.hpp"
    "include/ProtocolService.hpp"
    "include/StreamRenderer.hpp"
    "include/ThreadPool.hpp")

set(STREAMSIM_SOURCE_FILES
    "src/DemoDecoder.cpp"
    "src/DemoNetInputStream.cpp"
    "src/DemoProtocolService.cpp"
    "src/DemoRenderer.cpp")

add_library(StreamSimulation ${STREAMSIM_SOURCE_FILES} ${STREAMSIM_HEADER_FILES})

target_include_directories(StreamSimulation PUBLIC ${STREAM_INCLUDE_DIR})

install(TARGETS StreamSimulation DESTINATION lib)
install(FILES ${STREAMSIM_HEADER_FILES} DESTINATION include/StreamSimulation)

add_subdirectory(tests)
