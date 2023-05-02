#pragma once

#include <tr/unimplemented.h>
#include <cstddef>

namespace tr {

template <typename>
struct at_impl : unimplemented {
    template <typename Iterable, typename Idx>
    static auto apply(Iterable &&, Idx) = delete;
};

struct at_t {
    template <typename Iterable, typename Idx>
    [[nodiscard]] constexpr decltype(auto) operator()(Iterable &&, Idx) const;
};

static constexpr at_t at{};

template <std::size_t N, typename Iterable>
[[nodiscard]] constexpr decltype(auto) at_c(Iterable &&);

} // namespace tr