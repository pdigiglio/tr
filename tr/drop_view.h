#pragma once

#include "./fwd/drop_view.h"

#include "./at.h"
#include "./length.h"
#include "./tuple_view.h"

#include <utility>

namespace tr {

namespace detail {

template <std::size_t N, std::size_t... Is>
constexpr auto drop_indices_for_impl(std::index_sequence<Is...>) noexcept
    -> std::index_sequence<N + Is...> {
    return {};
}

template <std::size_t DropCount, typename Tuple>
constexpr auto drop_indices_for(Tuple &&tuple) noexcept {

    auto tupleLength = length(std::forward<Tuple>(tuple));
    if constexpr (DropCount <= tupleLength) {

        using index_sequence_t =
            std::make_index_sequence<tupleLength - DropCount>;
        return drop_indices_for_impl<DropCount>(index_sequence_t{});

    } else {
        //static_assert(value_c<false>.value, "Dropping too many elements");
    }
}

template <typename Tuple, std::size_t DropCount>
using drop_indices_for_t =
    decltype(drop_indices_for<DropCount>(std::declval<Tuple>()));

} // namespace detail

template <typename Tuple, std::size_t DropCount>
struct drop_view
    : tuple_view<Tuple, detail::drop_indices_for_t<Tuple, DropCount>> {

    using base_t =
        tuple_view<Tuple, detail::drop_indices_for_t<Tuple, DropCount>>;

    template <typename Tuple_>
    constexpr explicit drop_view(Tuple_ &&tuple) noexcept(
        std::is_nothrow_constructible_v<base_t, Tuple_ &&>)
        : base_t(std::forward<Tuple_>(tuple)) {}
};

template <typename Tuple, std::size_t DropCount>
struct at_impl<drop_view<Tuple, DropCount>>
    : at_impl<typename drop_view<Tuple, DropCount>::base_t> {};

template <typename Tuple, std::size_t DropCount>
struct length_impl<drop_view<Tuple, DropCount>>
    : length_impl<typename drop_view<Tuple, DropCount>::base_t> {};

template <std::size_t N>
struct drop_t {};

template <std::size_t N>
static constexpr drop_t<N> drop_c{};

template <typename Tuple, std::size_t N>
[[nodiscard]] constexpr auto operator|(Tuple &&tuple, drop_t<N>)
    -> drop_view<Tuple, N> {
    return drop_view<Tuple, N>{std::forward<Tuple>(tuple)};
}

} // namespace tr