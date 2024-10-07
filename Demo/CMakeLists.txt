add_executable(StreamDemo main.cpp)

target_include_directories(StreamDemo PUBLIC ${STREAM_INCLUDE_DIR})
target_link_libraries(StreamDemo StreamSimulation)