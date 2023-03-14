#pragma once

#include "../tuple_protocol.h"
#include "../value_constant.h"

#include <cstddef>
#include <utility>

namespace tr {
template <typename T, T... Is>
struct tup_size<std::integer_sequence<T, Is...>>
    : std::integral_constant<std::size_t, sizeof...(Is)> {};

template <std::size_t J, typename T, T... Is>
[[nodiscard]] constexpr auto get(std::integer_sequence<T, Is...>) noexcept {
    static_assert(J < sizeof...(Is), "Index out of bounds");
    constexpr std::size_t indices[]{Is...};
    return value_c<indices[J]>;
}
} // namespace tr