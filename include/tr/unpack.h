#pragma once

#include <tr/fwd/unpack.h>

#include <tr/at.h>
#include <tr/detail/type_traits.h>
#include <tr/indices_for.h>
#include <tr/length.h>

#include <type_traits>
#include <utility>

namespace tr {

template <typename Tuple, typename Func>
constexpr auto unpack_impl_t ::operator()(Tuple &&t, Func &&f) const
    -> decltype(auto) {
    using tuple_t = detail::remove_cvref_t<Tuple>;
    return unpack_impl<tuple_t>::apply(std::forward<Tuple>(t),
                                       std::forward<Func>(f));
}

template <typename Tuple>
struct unpack_impl<Tuple,
                   std::enable_if_t<is_implemented_v<at_impl<Tuple>> &&
                                    is_implemented_v<length_impl<Tuple>>>> {

    template <typename Tuple_, typename Func, std::size_t... Is>
    static constexpr auto apply_impl(Tuple_ &&t, Func &&f,
                                     std::index_sequence<Is...>)
        -> decltype(auto) {
        return std::forward<Func>(f)(at_c<Is>(std::forward<Tuple_>(t))...);
    }

    template <typename Tuple_, typename Func>
    static constexpr auto apply(Tuple_ &&t, Func &&f) -> decltype(auto) {
        return unpack_impl::apply_impl(std::forward<Tuple_>(t),
                                       std::forward<Func>(f), indices_for(t));
    }
};

} // namespace tr