cmake_minimum_required(VERSION 3.31)
project(OrderBook)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

add_executable(odb UnitTests/OrderBookAndRecordsTest.cpp)
#


find_package(GTest CONFIG REQUIRED)
target_link_libraries(odb PUBLIC GTest::gtest GTest::gtest_main)

target_include_directories(odb PUBLIC "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/src"

)



