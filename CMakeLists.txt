cmake_minimum_required(VERSION 3.31)
project(OrderBook)

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
        "${PROJECT_SOURCE_DIR}/include/OverwritingVector"
        "${PROJECT_SOURCE_DIR}/include/Matcher"
        "${PROJECT_SOURCE_DIR}/include/Order"
        "${PROJECT_SOURCE_DIR}/include/RecordDepot"
        "${PROJECT_SOURCE_DIR}/include/OrderBook"
        "${PROJECT_SOURCE_DIR}/include/Instrument"
        "${PROJECT_SOURCE_DIR}/include/ValueTypes"
        "${PROJECT_SOURCE_DIR}/include/Generator"
        )



#add_executable(demo demo.cpp  ${SOURCE})
add_executable(demo tests/PerformanceTests/ThroughputTests/ThroughputTest.cpp  ${SOURCE})
target_include_directories(demo PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )

add_executable(benchmarks
        tests/PerformanceTests/BenchmarkTests/FifoBenchTest.cpp
        tests/PerformanceTests/BenchmarkTests/OrderProcessingTest.cpp
        ${SOURCE})
target_include_directories(benchmarks PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark benchmark::benchmark_main)


string(TIMESTAMP current_time_utc UTC)
add_custom_target(save_benchmarks
        COMMAND $<TARGET_FILE:benchmarks>
        "--benchmark_out=${CMAKE_CURRENT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkReports/FifoBench_${current_time_utc}"
        "--benchmark_out_format=json"
        VERBATIM
)
add_dependencies(save_benchmarks benchmarks)






set(TESTS

        tests/UnitTests/RecordDepotTests.cpp
           tests/UnitTests/OverwritingVectorTests.cpp
            tests/UnitTests/FifoTests.cpp
           tests/UnitTests/RecordDepotTests.cpp
        tests/IntegrationTests/OrderBookTests.cpp
)

add_executable(tests ${SOURCE} ${TESTS} )
target_include_directories(demo PUBLIC ${INCLUDE})

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PUBLIC GTest::gtest GTest::gtest_main )
target_include_directories(tests PUBLIC ${INCLUDE})