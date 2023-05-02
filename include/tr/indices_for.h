#pragma once

#include <tr/fwd/indices_for.h>

#include <tr/detail/type_traits.h>
#include <tr/length.h>

#include <type_traits>
#include <utility>

namespace tr {

template <typename T>
struct indices_for_impl<T,
                     std::enable_if_t<is_implemented_v<length_impl<T>>>> {

    template <typename Sized>
    [[nodiscard]] static constexpr auto apply(Sized &&sized) noexcept {
        auto size = tr::length(sized);
        return std::make_index_sequence<decltype(size)::value>{};
    }
};

template <typename Sized>
[[nodiscard]] constexpr decltype(auto)
indices_for_t::operator()(Sized &&sized) const {
    using sized_t = detail::remove_cvref_t<Sized>;
    return indices_for_impl<sized_t>::apply(sized);
}

} // namespace tr