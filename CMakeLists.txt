cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

add_library(tr INTERFACE)
target_include_directories(tr INTERFACE include)
target_compile_features(tr INTERFACE cxx_std_17)

# Add include/ subdir to have VS show the header files.
add_subdirectory(include)
add_subdirectory(tests)
