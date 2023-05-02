#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct unpack_impl : unimplemented {
    template <typename Tuple, typename Func>
    static auto apply(Tuple &&, Func &&) = delete;
};

struct unpack_impl_t {
    template <typename Tuple, typename Func>
    constexpr auto operator()(Tuple &&, Func &&) const -> decltype(auto);
};

static constexpr unpack_impl_t unpack{};
} // namespace tr
