#pragma once

#include "../tuple_protocol.h"

#include <tuple>
#include <utility>

namespace tr {
template <typename... Ts>
struct tup_size<std::tuple<Ts...>> : std::tuple_size<std::tuple<Ts...>> {};

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(std::tuple<Ts...> const &t) noexcept {
    return std::get<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(std::tuple<Ts...> &t) noexcept {
    return std::get<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(std::tuple<Ts...> &&t) noexcept {
    return std::get<I>(std::move(t));
}
} // namespace tr
