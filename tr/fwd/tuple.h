#pragma once

#include <cstddef>

namespace tr {

namespace detail {

/// @brief (Unimplemented) tag type for tuple elements.
template <std::size_t>
struct tuple_tag;

template <typename, std::size_t, typename = void>
struct tup_elem_base;

template <typename, std::size_t>
struct tup_elem;

template <typename TL, typename IL>
struct tuple_base;

template <typename, typename>
struct tuple_swap_trait;

} // namespace detail

template <typename...>
struct tuple;

} // namespace tr
