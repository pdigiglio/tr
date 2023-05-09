#pragma once

#include <tr/algorithm/fwd/fold_left_first.h>

#include <tr/at.h>
#include <tr/detail/utility.h>
#include <tr/indices_for.h>

namespace tr {

namespace detail {

template <typename Indices>
struct fold_left_first_impl;

template <std::size_t... Is>
struct fold_left_first_impl<std::index_sequence<0, Is...>> {
    // Make sure I have at least one element ---^

    template <typename Tuple, typename Combinator>
    static constexpr auto apply(Tuple &&t, Combinator &&c) -> decltype(auto) {

        auto &&init = at_c<0>(static_cast<Tuple &&>(t));
        combinator comb{static_cast<Combinator &&>(c),
                        static_cast<decltype(init)>(init)};

        auto foldExpr =
            (std::move(comb) | ... | at_c<Is>(static_cast<Tuple &&>(t)));
        return detail::lref_or_value(std::move(foldExpr).value());
    }
};

template <typename T>
using indices_for_t = decltype(indices_for(std::declval<T>()));

} // namespace detail

template <typename T>
struct fold_left_first_impl<
    T, std::void_t<decltype(
           detail::fold_left_first_impl<detail::indices_for_t<T>>{})>>
    : detail::fold_left_first_impl<detail::indices_for_t<T>> {};

template <typename NonEmptyTuple, typename Combinator>
[[nodiscard]] constexpr auto
fold_left_first_t::operator()(NonEmptyTuple &&tuple, Combinator &&c) const
    -> decltype(auto) {

    using tuple_t = detail::remove_cvref_t<NonEmptyTuple>;
    return fold_left_first_impl<tuple_t>::apply(
        static_cast<NonEmptyTuple &&>(tuple), static_cast<Combinator &&>(c));
}

} // namespace tr