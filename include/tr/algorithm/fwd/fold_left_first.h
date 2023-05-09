#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct fold_left_first_impl : unimplemented {
    template <typename NonEmptyTuple, typename Combinator>
    static auto apply(NonEmptyTuple &&, Combinator &&) = delete;
};

struct fold_left_first_t {
    /// @brief Perform a left fold on a tuple-like object using its first
    /// element as initial value.
    /// @tparam NonEmptyTuple The type of the tuple-like object.
    /// @tparam Combinator The combinator type (i.e. the binary operator to fold
    /// onto).
    /// @param tuple The tuple-like object.
    /// @param c The binary operator.
    /// @return The result of `c(c(c(tuple[0_ic], tuple[1_ic]), tuple[2_ic])...)`.
    template <typename NonEmptyTuple, typename Combinator>
    [[nodiscard]] constexpr auto operator()(NonEmptyTuple &&,
                                            Combinator &&) const
        -> decltype(auto);
};

static constexpr fold_left_first_t fold_left_first{};

} // namespace tr