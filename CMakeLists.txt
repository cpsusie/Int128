cmake_minimum_required(VERSION 3.17)
project(Int128)

set(CMAKE_CXX_STANDARD 20)

include_directories(cpp/src/cpp)
include_directories(cpp/src/headers)
find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
    add_executable(Int128
        cpp/src/cpp/program.cpp
        cpp/src/cpp/testing.cpp
        cpp/src/cpp/int128_tests.cpp
        )