#pragma once

#include "../fwd/at.h"
#include "../fwd/length.h"

#include "../tuple_protocol.h"

#include <utility>

namespace tr {

template <typename First, typename Second>
struct length_impl<std::pair<First, Second>> {
    template <typename Pair>
    [[nodiscard]] constexpr static auto apply(Pair &&) noexcept
        -> value_constant<2> {
        return {};
    }
};

template <typename First, typename Second>
struct at_impl<std::pair<First, Second>> {

    template <typename Pair, typename Idx>
    [[nodiscard]] constexpr static auto apply(Pair &&pair, Idx) noexcept
        -> decltype(auto) {
        return std::get<Idx{}>(std::forward<Pair>(pair));
    }
};

template <typename... Ts>
struct tup_size<std::pair<Ts...>> : std::tuple_size<std::pair<Ts...>> {};

template <std::size_t I, typename... Ts>
[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...> const &t) noexcept {
    return std::get<I>(t);
}

template <std::size_t I, typename... Ts>
[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...> &t) noexcept {
    return std::get<I>(t);
}

template <std::size_t I, typename... Ts>
[[nodiscard]] constexpr decltype(auto) get(std::pair<Ts...> &&t) noexcept {
    return std::get<I>(std::move(t));
}
} // namespace tr
