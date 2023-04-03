#pragma once

#include "../unimplemented.h"

namespace tr {

template <typename, typename = void>
struct all_of_impl : unimplemented {
    template <typename Tuple, typename Predicate>
    static auto apply(Tuple &&, Predicate &&) = delete;
};

struct all_of_t {
    template <typename Tuple, typename Predicate>
    [[nodiscard]] constexpr auto operator()(Tuple &&, Predicate &&) const
        -> decltype(auto);
};

static constexpr all_of_t all_of{};

} // namespace tr