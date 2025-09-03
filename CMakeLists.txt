cmake_minimum_required(VERSION 3.31)
project(OrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(SOURCE
        "${PROJECT_SOURCE_DIR}/src/Matcher/Fifo.cpp"
        "${PROJECT_SOURCE_DIR}/src/OrderBook/Evaluator.cpp"
        "${PROJECT_SOURCE_DIR}/src/RecordDepot/Record.cpp"
        "${PROJECT_SOURCE_DIR}/src/Generator/OrderGenerator.cpp"
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

# Demo Project
add_executable(demo demo.cpp  ${SOURCE})
target_include_directories(demo PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/include/Printer" )


# Unit Tests
set(TESTS
        "${PROJECT_SOURCE_DIR}/tests/UnitTests/RecordDepotTests.cpp"
        "${PROJECT_SOURCE_DIR}/tests/UnitTests/OverwritingVectorTests.cpp"
        "${PROJECT_SOURCE_DIR}/tests/UnitTests/FifoTests.cpp"
        "${PROJECT_SOURCE_DIR}/tests/UnitTests/RecordDepotTests.cpp"
        "${PROJECT_SOURCE_DIR}/tests/IntegrationTests/OrderBookTests.cpp"
)

add_executable(tests ${SOURCE} ${TESTS} )
target_include_directories(tests PUBLIC ${INCLUDE})

find_package(GTest CONFIG REQUIRED)
target_link_libraries(tests PUBLIC GTest::gtest GTest::gtest_main )


# Benchmark Tests
set(BENCHMARKS
        "${PROJECT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkTests/EvaluateAndMatch.cpp"
       # "${PROJECT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkTests/FifoBenchTest.cpp"
)


add_executable(benchmarks ${BENCHMARKS} ${SOURCE})
target_include_directories(benchmarks PUBLIC
        ${INCLUDE}
        "${PROJECT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkTests/include"
        "${PROJECT_SOURCE_DIR}/include/Printer" )

find_package(benchmark CONFIG REQUIRED)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark benchmark::benchmark_main)

# Save benchmarks to file
string(TIMESTAMP current_time_utc UTC)
add_custom_target(save_benchmarks
        COMMAND $<TARGET_FILE:benchmarks>
        "--benchmark_out=${CMAKE_CURRENT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkReports/FifoBench_${current_time_utc}"
        "--benchmark_out_format=json"
        VERBATIM
)
add_dependencies(save_benchmarks benchmarks)

# Throughput Tests
set(THROUGHPUT
       "${PROJECT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkTests/FullProcessingPath.cpp"
)

add_executable(throughput ${THROUGHPUT} ${SOURCE})
target_include_directories(throughput PUBLIC ${INCLUDE} "${PROJECT_SOURCE_DIR}/tests/PerformanceTests/BenchmarkTests/include")



