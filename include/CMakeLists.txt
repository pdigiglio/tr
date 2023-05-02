if (MSVC)
    set(TR_SOURCE_LIST
        tr/algorithm.h
        tr/algorithm/all_of.h
        tr/algorithm/for_each.h
        tr/algorithm/fwd/all_of.h
        tr/algorithm/fwd/for_each.h
        tr/as_array.h
        tr/at.h
        tr/combinator.h
        tr/detail/ebo.h
        tr/detail/flat_array.h
        tr/detail/literal_parser.h
        tr/detail/tuple_traits_utils.h
        tr/detail/type_traits.h
        tr/detail/utility.h
        tr/drop_view.h
        tr/forward_as_base.h
        tr/fwd/at.h
        tr/fwd/drop_view.h
        tr/fwd/indices_for.h
        tr/fwd/is_valid.h
        tr/fwd/length.h
        tr/fwd/reverse_view.h
        tr/fwd/tuple.h
        tr/fwd/type_pack.h
        tr/fwd/unimplemented.h
        tr/fwd/unpack.h
        tr/fwd/value_constant.h
        tr/fwd/view_interface.h
        tr/indices_for.h
        tr/is_valid.h
        tr/lazy_false.h
        tr/length.h
        tr/overloaded.h
        tr/reverse_view.h
        tr/tuple.h
        tr/tuple_protocol.h
        tr/tuple_protocol/built_in_array.h
        tr/tuple_protocol/std_integer_sequence.h
        tr/tuple_protocol/std_pair.h
        tr/tuple_protocol/std_tuple.h
        tr/tuple_view.h
        tr/type_constant.h
        tr/type_identity.h
        tr/type_pack.h
        tr/unimplemented.h
        tr/unpack.h
        tr/value_constant.h
        tr/value_sequence.h
        tr/view_interface.h)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TR_SOURCE_LIST})
    # [VS] Add target just to have a list of tr's header files.
    add_executable(tr.headers ${TR_SOURCE_LIST} dummy.cpp)
    target_link_libraries(tr.headers PRIVATE tr)
    #target_include_directories(tr.headers PRIVATE .)
endif()
