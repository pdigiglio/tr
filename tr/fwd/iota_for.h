#pragma once

#include "../unimplemented.h"

namespace tr {

template <typename, typename = void>
struct iota_for_impl : unimplemented {
    template <typename Sized>
    static auto apply(Sized &&) = delete;
};

/// @brief Create a sequence of indices for a `Sized` object (i.e. an
/// object whose number of elements is a constant expression).
struct iota_for_t {
    template <typename Sized>
    [[nodiscard]] constexpr decltype(auto) operator()(Sized &&) const;
};

static constexpr iota_for_t iota_for{};

} // namespace tr