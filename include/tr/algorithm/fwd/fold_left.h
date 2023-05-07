#pragma once

#include <tr/unimplemented.h>

namespace tr {

template <typename, typename = void>
struct fold_left_impl : unimplemented {
    template <typename Tuple, typename Init, typename Combinator>
    static auto apply(Tuple &&, Init&&, Combinator &&) = delete;
};

struct fold_left_t {

    /// @brief Perform a left fold on a tuple-like object.
    /// @tparam Tuple The type of the tuple-like object.
    /// @tparam Combinator The combinator type (i.e. the binary operator to fold onto).
    /// @tparam Init The type of the initial value.
    /// @param  tuple The tuple-like object.
    /// @param  init The initial value.
    /// @param  c The binary operator.
    /// @return The result of `c(c(c(init, tuple[0_ic]), tuple[1_ic])...)`.
    template <typename Tuple, typename Init, typename Combinator>
    [[nodiscard]] constexpr auto operator()(Tuple &&, Init &&,
                                            Combinator &&) const
        -> decltype(auto);
};

static constexpr fold_left_t fold_left{};

} // namespace tr