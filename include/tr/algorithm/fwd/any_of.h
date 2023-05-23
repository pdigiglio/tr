#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename T, typename = void>
struct any_of_impl : unimplemented {

    template <typename Tuple, typename Predicate>
    static auto apply(Tuple &&, Predicate &&) = delete;
};

struct any_of_t {
    template <typename Tuple, typename Predicate>
    [[nodiscard]] constexpr auto operator()(Tuple &&tuple,
                                            Predicate &&predicate) const
        -> decltype(auto);
};

static constexpr any_of_t any_of{};

} // namespace tr