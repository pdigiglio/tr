#pragma once

#include "./fwd/at.h"

#include "./detail/type_traits.h"
#include "./detail/utility.h"

#include <utility>

namespace tr {

template <typename Iterable, typename Idx>
[[nodiscard]] constexpr decltype(auto) at_t::operator()(Iterable &&iterable,
                                                        Idx idx) const {
    using iterable_t = detail::remove_cvref_t<Iterable>;
    return at_impl<iterable_t>::apply(std::forward<Iterable>(iterable), idx);
}

template <std::size_t N, typename Iterable>
[[nodiscard]] constexpr decltype(auto) at_c(Iterable &&iterable) {
    return at(std::forward<Iterable>(iterable),
              std::integral_constant<std::size_t, N>{});
}

template <typename T, std::size_t N>
struct at_impl<T[N]> {
    template <typename Iterable, typename Idx>
    static constexpr decltype(auto) apply(Iterable &&arr, Idx) noexcept {
        static_assert(Idx{} < N, "Index out of bounds");
        return detail::forward_like<Iterable>(arr[Idx{}]);
    }
};

} // namespace tr
