#pragma once

#include "./fwd/reverse_view.h"

#include "./indices_for.h"
#include "./tuple_view.h"

namespace tr {

namespace detail {

template <std::size_t... Is>
auto reverse_indices_impl(std::index_sequence<Is...>)
    -> std::index_sequence<(sizeof...(Is) - Is - 1)...> /* unimplemented */;

template <typename Tuple>
auto reverse_tuple_indices_impl(Tuple &&tuple)
    -> decltype(reverse_indices_impl(indices_for(tuple))) /* unimplemented */;

template <typename Tuple>
using reverse_indices_for_t = decltype(reverse_tuple_indices_impl(std::declval<Tuple>()));
} // namespace detail

template <typename Tuple>
struct reverse_view : tuple_view<Tuple, detail::reverse_indices_for_t<Tuple>> {
    using base_t = tuple_view<Tuple, detail::reverse_indices_for_t<Tuple>>;

    template <typename Tuple_>
    constexpr explicit reverse_view(Tuple_ &&tuple) noexcept(
        std::is_nothrow_constructible_v<base_t, Tuple_ &&>)
        : base_t(std::forward<Tuple_>(tuple)) {}
};

template <typename Tuple>
struct at_impl<reverse_view<Tuple>>
    : at_impl<typename reverse_view<Tuple>::base_t> {};

template <typename Tuple>
struct length_impl<reverse_view<Tuple>>
    : length_impl<typename reverse_view<Tuple>::base_t> {};

struct reverse_t {};

static constexpr reverse_t reverse{};

template <typename Tuple>
[[nodiscard]] constexpr auto operator|(Tuple &&tuple, reverse_t)
    -> reverse_view<Tuple> {
    return reverse_view<Tuple>{std::forward<Tuple>(tuple)};
}

} // namespace tr