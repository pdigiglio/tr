#pragma once

#include "../fwd/at.h"
#include "../fwd/length.h"

#include "../tuple_protocol.h"
#include "../value_constant.h"

#include <cstddef>
#include <utility>

namespace tr {

template <typename T, T... Is>
struct at_impl<std::integer_sequence<T, Is...>> {

    template <typename Iterable, typename Idx>
    static constexpr auto apply(Iterable &&, Idx) noexcept -> decltype(auto) {
        static_assert(Idx{} < sizeof...(Is), "Index out of bounds");
        constexpr T is[]{Is...};
        return value_c<is[Idx{}]>;
    }
};

template <typename T, T... Is>
struct length_impl<std::integer_sequence<T, Is...>> {

    template <typename Sized>
    static constexpr auto apply(Sized &&) noexcept
        -> value_constant<sizeof...(Is)> {
        return {};
    }
};

// TODO: remove this --
template <typename T, T... Is>
struct tup_size<std::integer_sequence<T, Is...>>
    : std::integral_constant<std::size_t, sizeof...(Is)> {};

template <std::size_t J, typename T, T... Is>
[[nodiscard]] constexpr auto get(std::integer_sequence<T, Is...>) noexcept {
    static_assert(J < sizeof...(Is), "Index out of bounds");
    constexpr std::size_t indices[]{Is...};
    return value_c<indices[J]>;
}
// --

} // namespace tr