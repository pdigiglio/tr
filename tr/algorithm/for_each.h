#pragma once

#include "fwd/for_each.h"

#include "../fwd/at.h"
#include "../fwd/indices_for.h"
#include "../detail/type_traits.h"

#include <utility>

namespace tr {

template <typename T, typename UnaryFunc>
constexpr UnaryFunc &&for_each_t::operator()(T &&t, UnaryFunc &&pred) const {
    using t_t = detail::remove_cvref_t<T>;
    return for_each_impl<t_t>::apply(std::forward<T>(t),
                                     std::forward<UnaryFunc>(pred));
}

template <typename Tuple>
struct for_each_impl<Tuple,
                     std::enable_if_t<is_implemented_v<indices_for_impl<Tuple>> &&
                                      is_implemented_v<at_impl<Tuple>>>> {

    template <typename Tuple_, typename UnaryFunc, std::size_t... Is>
    static constexpr UnaryFunc &&apply_impl(Tuple_ &&t, UnaryFunc &&pred,
                                       std::index_sequence<Is...>) {
        (std::forward<UnaryFunc>(pred)(at_c<Is>(std::forward<Tuple_>(t))), ...);
        return std::forward<UnaryFunc>(pred);
    }

    template <typename Tuple_, typename UnaryFunc>
    static constexpr UnaryFunc &&apply(Tuple_ &&t, UnaryFunc &&pred) {
        return for_each_impl::apply_impl(
            std::forward<Tuple_>(t), std::forward<UnaryFunc>(pred), indices_for(t));
    }
};

} // namespace tr