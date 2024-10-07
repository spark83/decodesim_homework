cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(FrameProcess VERSION 1.0)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(STREAM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/StreamSimulation/include")

# Add the executable
add_subdirectory(StreamSimulation)
add_subdirectory(Demo)
