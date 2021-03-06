cmake_minimum_required(VERSION 3.17)
project(Int128)

set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -Wextra -Wstrict-aliasing -Wno-unknown-pragmas -Wstrict-overflow -madx -mbmi2 -msse3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_VERBOSE_MAKEFILE ON)
include_directories(uint128_test_app/src/headers)
include_directories(./src/include)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
find_package(absl REQUIRED)
IF (absl_FOUND)
    include_directories(${absl_INCLUDE_DIRS})
endif()

add_executable(Int128
    uint128_test_app/src/cpp/program.cpp
    uint128_test_app/src/cpp/testing.cpp
    uint128_test_app/src/cpp/int128_tests.cpp
    uint128_test_app/src/cpp/int128_test_switches.cpp
    uint128_test_app/src/cpp/uint128_test_program.cpp
    uint128_test_app/src/cpp/test_runner.cpp
    uint128_test_app/src/cpp/output_guard.cpp
    )
target_link_libraries(Int128 absl::numeric pthread)

add_executable(EnvironTest
        visual_studio_environment_test/src/visual_studio_environment_test_app/program.cpp
        visual_studio_environment_test/src/visual_studio_environment_test_app/test_program.cpp
        visual_studio_environment_test/src/visual_studio_environment_test_app/test_intrinsics.cpp)

add_executable(ExampleCode
        example_code/src/uint128_example/program.cpp)
