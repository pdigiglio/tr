#pragma once

#include <tr/fwd/length.h>

#include <tr/detail/type_traits.h>
#include <tr/value_constant.h>

#include <utility>

namespace tr {

template <typename Sized>
[[nodiscard]] constexpr decltype(auto)
length_t::operator()(Sized &&sized) const {
    using sized_t = detail::remove_cvref_t<Sized>;
    return length_impl<sized_t>::apply(std::forward<Sized>(sized));
}

template <typename T, std::size_t N>
struct length_impl<T[N]> {
    template <typename Sized>
    [[nodiscard]] constexpr static value_constant<N> apply(Sized &&) noexcept {
        return {};
    }
};

} // namespace tr