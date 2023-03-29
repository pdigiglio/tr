#pragma once

#include "../unimplemented.h"

namespace tr {

template <typename>
struct length_impl : unimplemented {
    template <typename Sized>
    static auto apply(Sized &&) = delete;
};

/// @brief Function object to determine the size of a compile-time sized
/// tuple-like object.
struct length_t {
    template <typename Sized>
    [[nodiscard]] constexpr decltype(auto) operator()(Sized &&) const;
};

static constexpr length_t length{};

} // namespace tr