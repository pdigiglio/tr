cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

set(SOURCE_LIST
    tr/algorithm.h
    tr/combinator.h
    tr/detail/ebo.h
    tr/detail/type_pack.h
    tr/detail/type_traits.h
    tr/detail/utility.h
    tr/overload.h
    tr/tuple.h
    tr/tuple_protocol.h
    tr/tuple_protocol/built_in_array.h
    tr/tuple_protocol/std_integer_sequence.h
    tr/tuple_protocol/std_pair.h
    tr/tuple_protocol/std_tuple.h
    tr/tuple_view.h
    tr/type_constant.h
    tr/value_constant.h
    tr/value_sequence.h
    main.cpp)

add_executable(${PROJ_NAME} ${SOURCE_LIST})
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

if (MSVC)
    target_compile_options(${PROJ_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJ_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${PROJ_NAME} PRIVATE -Wno-missing-braces)
    endif()
endif()
