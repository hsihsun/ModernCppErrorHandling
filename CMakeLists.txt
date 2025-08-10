cmake_minimum_required(VERSION 3.22)
project(smart_surgery_tek_demo CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CTEST_OUTPUT_ON_FAILURE ON)

find_package(GTest REQUIRED)

add_library(pipeline STATIC
    pipeline.cpp
    pipeline.h
)
target_include_directories(pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(pipeline_tests
    pipeline_test.cpp
)
target_link_libraries(pipeline_tests PRIVATE pipeline GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(pipeline_tests)
