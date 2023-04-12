#pragma once

#include "../fwd/tuple.h"
#include "../fwd/type_pack.h"

#include <type_traits>

namespace tr {
namespace detail {

template <typename, typename>
static constexpr bool are_tuples_swappable_v{false};

template <typename T, typename U, std::size_t I>
static constexpr bool are_tuples_swappable_v<tup_elem<T, I>, tup_elem<U, I>>{
    std::is_swappable_with_v<std::add_lvalue_reference_t<T>,
                             std::add_lvalue_reference_t<U>>};

template <typename... Ts, typename... Us, std::size_t... Is>
static constexpr bool are_tuples_swappable_v<
    tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>,
    tuple_base<type_pack<Us...>, std::index_sequence<Is...>>>{(
    are_tuples_swappable_v<tup_elem<Ts, Is>, tup_elem<Us, Is>> && ... && true)};

template <typename>
static constexpr bool is_tuple_swappable_v{false};

template <typename... Ts>
static constexpr bool is_tuple_swappable_v<tuple<Ts...>>{are_tuples_swappable_v<
    tuple_base<type_pack<Ts...>, std::index_sequence_for<Ts...>>,
    tuple_base<type_pack<Ts...>, std::index_sequence_for<Ts...>>>};

template <typename, typename>
static constexpr bool are_tuples_nothrow_swappable_v{false};

template <typename T, typename U, std::size_t I>
static constexpr bool
    are_tuples_nothrow_swappable_v<tup_elem<T, I>, tup_elem<U, I>>{
        std::is_nothrow_swappable_with_v<std::add_lvalue_reference_t<T>,
                                         std::add_lvalue_reference_t<U>>};

template <typename... Ts, typename... Us, std::size_t... Is>
static constexpr bool are_tuples_nothrow_swappable_v<
    tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>,
    tuple_base<type_pack<Us...>, std::index_sequence<Is...>>>{
    (are_tuples_nothrow_swappable_v<tup_elem<Ts, Is>, tup_elem<Us, Is>> &&
     ... && true)};

} // namespace detail
} // namespace tr