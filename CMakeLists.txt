cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

add_executable(
    ${PROJ_NAME}
    folder.h
    detail/utility.h
    main.cpp)

target_compile_features(${PROJ_NAME} PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(${PROJ_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJ_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
