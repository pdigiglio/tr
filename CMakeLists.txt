cmake_minimum_required(VERSION 3.12.0)

set(PROJ_NAME "tr")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

add_library(tr INTERFACE)
target_compile_features(tr INTERFACE cxx_std_17)

if (MSVC)
    set(TR_SOURCE_LIST
        tr/algorithm.h
        tr/all_of.h
        tr/at.h
        tr/combinator.h
        tr/detail/ebo.h
        tr/detail/type_pack.h
        tr/detail/type_traits.h
        tr/detail/utility.h
        tr/drop_view.h
        tr/for_each.h
        tr/forward_as_base.h
        tr/fwd/all_of.h
        tr/fwd/at.h
        tr/fwd/drop_view.h
        tr/fwd/for_each.h
        tr/fwd/indices_for.h
        tr/fwd/length.h
        tr/fwd/unimplemented.h
        tr/fwd/unpack.h
        tr/fwd/view_interface.h
        tr/indices_for.h
        tr/lazy_false.h
        tr/length.h
        tr/overloaded.h
        tr/tuple.h
        tr/tuple_protocol.h
        tr/tuple_protocol/built_in_array.h
        tr/tuple_protocol/std_integer_sequence.h
        tr/tuple_protocol/std_pair.h
        tr/tuple_protocol/std_tuple.h
        tr/tuple_view.h
        tr/type_constant.h
        tr/type_identity.h
        tr/unimplemented.h
        tr/unpack.h
        tr/value_constant.h
        tr/value_sequence.h
        tr/view_interface.h)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TR_SOURCE_LIST})
    # [VS] Add target just to have a list of tr's header files.
    add_custom_target(
        tr_headers
        SOURCES ${TR_SOURCE_LIST}
        DEPENDS tr)
endif()

# Doesn't work
#set(CMAKE_CXX_CLANG_TIDY "clang-tidy.exe;-checks='modernize-*'")

add_subdirectory(tests)
