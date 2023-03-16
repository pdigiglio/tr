cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

add_library(tr INTERFACE)
target_compile_features(tr INTERFACE cxx_std_17)

if (MSVC)
    set(TR_SOURCE_LIST
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
        tr/value_sequence.h)

    # [VS] Add project just to have a list of tr's header files.
    add_executable(tr_headers ${TR_SOURCE_LIST})
    target_link_libraries(tr_headers PRIVATE tr) 
    set_target_properties(tr_headers PROPERTIES LINKER_LANGUAGE CXX)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TR_SOURCE_LIST})
endif()

add_subdirectory(tests)
