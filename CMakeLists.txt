cmake_minimum_required(VERSION 2.8)
project(mpack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")

add_subdirectory(gtest)
add_subdirectory(tests)
add_subdirectory(asio_sample)

