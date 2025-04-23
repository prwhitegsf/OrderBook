cmake_minimum_required(VERSION 3.31)
project(OrderBook)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

add_executable(odb main.cpp)
#UnitTests/ClientOrderTypeTests.cpp
#[[add_subdirectory(OrderTypes)
add_subdirectory(Instrument)
add_subdirectory(Matchers/FIFO)
add_subdirectory(Orderbook)
add_subdirectory(PriceLadders/FixedSizeLadder)
add_subdirectory(PriceLevels/DequeLevel)
]]



target_include_directories(odb PUBLIC "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/Instrument"
        "${PROJECT_SOURCE_DIR}/OrderTypes"
        "${PROJECT_SOURCE_DIR}/Orderbook"
        "${PROJECT_SOURCE_DIR}/PriceLadders/FixedSizeLadder"
        "${PROJECT_SOURCE_DIR}/PriceLevels/DequeLevel"
        "${PROJECT_SOURCE_DIR}/Matchers/FIFO"
        "${PROJECT_SOURCE_DIR}/Instrument/OrderLists"
        "${PROJECT_SOURCE_DIR}/Printer"
        "${PROJECT_SOURCE_DIR}/Generators"
)


#[[
find_package(GTest CONFIG REQUIRED)
target_link_libraries(odb PUBLIC GTest::gtest GTest::gtest_main)]]
