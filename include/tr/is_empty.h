#pragma once

#include <tr/fwd/is_empty.h>

#include <tr/length.h>
#include <tr/value_constant.h>

namespace tr {

template <typename T>
struct is_empty_impl<T, std::enable_if_t<is_implemented_v<length_impl<T>>>> {

    template <typename Tuple>
    static constexpr auto apply(Tuple &&tuple) -> decltype(auto) {
        return length(tuple) == value_c<0>;
    }
};

template <typename Tuple>
[[nodiscard]] constexpr auto is_empty_t::operator()(Tuple &&tuple) const
    -> decltype(auto) {
    using tuple_t = detail::remove_cvref_t<Tuple>;
    return is_empty_impl<tuple_t>::apply(static_cast<Tuple &&>(tuple));
}

} // namespace tr