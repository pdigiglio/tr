#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct for_each_impl : unimplemented {
    template <typename T, typename UnaryFunc>
    static auto apply(T &&, UnaryFunc &&) = delete;
};

/// @brief Apply a `UnaryFunc` to each element in a sequence.
struct for_each_t {
    template <typename T, typename UnaryFunc>
    constexpr UnaryFunc &&operator()(T &&, UnaryFunc &&) const;
};

static constexpr for_each_t for_each{};

} // namespace tr