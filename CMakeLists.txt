cmake_minimum_required(VERSION 3.31)
project(OrderBook)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(SOURCE
        src/Matcher/Fifo.cpp
        src/OrderBook/Evaluator.cpp
        src/RecordDepot/Record.cpp
        src/Generator/OrderGenerator.cpp
        )

set(INCLUDE
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/include/Matcher"
        "${PROJECT_SOURCE_DIR}/include/Order"
        "${PROJECT_SOURCE_DIR}/include/RecordDepot"
        "${PROJECT_SOURCE_DIR}/include/OrderBook"
        "${PROJECT_SOURCE_DIR}/include/Instrument"
        "${PROJECT_SOURCE_DIR}/include/ValueTypes"
        "${PROJECT_SOURCE_DIR}/include/Generator"
        )



add_executable(demo demo.cpp ${SOURCE})
target_include_directories(demo PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )
##


add_executable(fifo_man tests/BenchmarksTests/FifoTestManual.cpp ${SOURCE})
target_include_directories(fifo_man PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )


add_executable(demo_bench tests/BenchmarksTests/DemoTest.cpp  ${SOURCE})

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(demo_bench PUBLIC benchmark::benchmark benchmark::benchmark_main)
target_include_directories(demo_bench PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )

add_custom_target(run_benchmark
        COMMAND $<TARGET_FILE:demo_bench>
        "--benchmark_out=${CMAKE_CURRENT_BINARY_DIR}/benchmark_results2.json"
        "--benchmark_out_format=json"
        VERBATIM
)

add_dependencies(run_benchmark demo_bench)


set(TESTS
    tests/UnitTests/FifoTests.cpp
    tests/UnitTests/RecordDepotTests.cpp
    tests/IntegrationTests/OrderBookTests.cpp)

add_executable(tests ${SOURCE} ${TESTS} )
target_include_directories(demo PUBLIC ${INCLUDE})

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PUBLIC GTest::gtest GTest::gtest_main )
target_include_directories(tests PUBLIC ${INCLUDE})