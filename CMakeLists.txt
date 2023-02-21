cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

set(SOURCE_LIST
    combinator.h
    detail/ebo.h
    detail/type_pack.h
    detail/type_traits.h
    detail/utility.h
    tuple.h
    main.cpp)

add_executable(${PROJ_NAME} ${SOURCE_LIST})
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

if (MSVC)
    target_compile_options(${PROJ_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJ_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
