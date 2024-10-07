
# Define your test executable
add_executable(test1 ConcurrentDataTest.cpp ThreadPoolTest.cpp)
add_executable(test2 DecoderTest.cpp)
add_executable(test3 NetInputStreamTest.cpp)
add_executable(test4 ProtocolServiceTest.cpp)
add_executable(test5 RendererTest.cpp)

target_include_directories(test1 PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(test1 StreamSimulation gtest gtest_main)

target_include_directories(test2 PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(test2 StreamSimulation gtest gtest_main)

target_include_directories(test3 PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(test3 StreamSimulation gtest gtest_main)

target_include_directories(test4 PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(test4 StreamSimulation gtest gtest_main)

target_include_directories(test5 PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(test5 StreamSimulation gtest gtest_main)

# Add test
add_test(NAME StreamSimTest COMMAND
         test1
         test2
         test3
         test4
         test5)
