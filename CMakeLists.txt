cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(tracer VERSION 1.0)
set(CMAKE_BUILD_TYPE Debug)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

file(GLOB ALL_FILES
     "src/*.cpp"
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include_directories("libs")

add_executable(${PROJECT_NAME} ${ALL_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads stdc++fs)
