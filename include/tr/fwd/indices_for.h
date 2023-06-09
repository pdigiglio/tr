#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct indices_for_impl : unimplemented {
    template <typename Sized>
    static auto apply(Sized &&) = delete;
};

/// @brief Create a sequence of indices for a `Sized` object (i.e. an
/// object whose number of elements is a constant expression).
struct indices_for_t {
    template <typename Sized>
    [[nodiscard]] constexpr decltype(auto) operator()(Sized &&) const;
};

static constexpr indices_for_t indices_for{};

} // namespace tr