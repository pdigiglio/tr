cmake_minimum_required(VERSION 3.14.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

add_library(tr INTERFACE)
target_include_directories(tr INTERFACE include)
target_compile_features(tr INTERFACE cxx_std_17)
    
    # Add include/ subdir to have VS show the header files.
option(LIBTR_COMPILE_TESTS "Build the trlib tests" OFF)

option(LIBTR_COMPILE_BENCH "Build the trlib benchmarks" OFF)

add_subdirectory(include)

if(LIBTR_COMPILE_TESTS)
    add_subdirectory(tests)
endif()

if(LIBTR_COMPILE_BENCH)
    add_subdirectory(benchmark)
endif()
