#pragma once

#include <tr/fwd/at.h>
#include <tr/fwd/length.h>

#include <tr/tuple_protocol.h>
#include <tr/value_constant.h>
#include <tr/view_interface.h>

#include <cstddef>
#include <utility>

namespace tr {
template <typename Tuple, typename IdxPack>
struct tuple_view;

template <typename Tuple, std::size_t... Is>
struct tuple_view<Tuple, std::index_sequence<Is...>>
    : view_interface<tuple_view<Tuple, std::index_sequence<Is...>>> {
  public:
    friend at_impl<tuple_view>;
    using index_sequence_t = std::index_sequence<Is...>;

    template <typename Tuple_>
    constexpr explicit tuple_view(Tuple_ &&tuple) noexcept(
        std::is_nothrow_constructible_v<Tuple, Tuple_ &&>)
        : tuple_(std::forward<Tuple_>(tuple)) {}

  private:
    Tuple tuple_;
};

template <typename Tuple, std::size_t... Is>
struct at_impl<tuple_view<Tuple, std::index_sequence<Is...>>> {

    template <typename TupleView, typename Idx>
    [[nodiscard]] static constexpr auto apply(TupleView &&tupleView,
                                              Idx idx)-> decltype(auto)
    {
        using index_sequence_t = std::index_sequence<Is...>;
        auto realIdx = at(index_sequence_t{}, idx);
        return at(std::forward<TupleView>(tupleView).tuple_, realIdx);
    }
};

template <typename Tuple, std::size_t... Is>
struct length_impl<tuple_view<Tuple, std::index_sequence<Is...>>> {

    template <typename TupleView>
    [[nodiscard]] static constexpr auto apply(TupleView &&) noexcept
        -> value_constant<sizeof...(Is)> {
        return {};
    }
};

//template <typename Tuple, std::size_t... Is>
//constexpr std::index_sequence<Is...>
//index_seq(tuple_view<Tuple, std::index_sequence<Is...>> const &) noexcept {
//    return {};
//};
//
//template <typename Tuple, std::size_t... Is>
//struct tup_size<tuple_view<Tuple, std::index_sequence<Is...>>>
//    : std::integral_constant<std::size_t, sizeof...(Is)> {};
//
//namespace detail {
//template <std::size_t I, typename TupleView>
//constexpr decltype(auto) tuple_view_get(TupleView &&tupleView) noexcept {
//    auto is = index_seq(tupleView);
//    static_assert(I < tup_size_v<decltype(is)>, "Index out of bounds");
//
//    using tr::get;
//
//    constexpr auto realIdx = get<I>(is);
//    return get<realIdx>(std::forward<TupleView>(tupleView).tuple_);
//}
//} // namespace detail
//
//template <std::size_t I, typename Tuple, std::size_t... Is>
//constexpr decltype(auto)
//get(tuple_view<Tuple, std::index_sequence<Is...>> const &tuple) noexcept {
//    return detail::tuple_view_get<I>(tuple);
//}
//
//template <std::size_t I, typename Tuple, std::size_t... Is>
//constexpr decltype(auto)
//get(tuple_view<Tuple, std::index_sequence<Is...>> &tuple) noexcept {
//    return detail::tuple_view_get<I>(tuple);
//}
//
//template <std::size_t I, typename Tuple, std::size_t... Is>
//constexpr decltype(auto)
//get(tuple_view<Tuple, std::index_sequence<Is...>> &&tuple) noexcept {
//    return detail::tuple_view_get<I>(std::move(tuple));
//}

template <typename, typename = void>
struct tuple_view_traits;

template <typename Tuple>
struct tuple_view_traits<Tuple, detail::void_t<decltype(tup_size_v<Tuple>)>> {
    using idx_seq = std::make_index_sequence<tup_size_v<Tuple>>;
};

template <typename Tuple, std::size_t... Is>
struct tuple_view_traits<tuple_view<Tuple, std::index_sequence<Is...>>, void> {
    using idx_seq = std::index_sequence<Is...>;
};

namespace detail {
//template <std::size_t N, std::size_t... Is>
//constexpr std::index_sequence<N + Is...>
//drop_first_idx_impl(std::index_sequence<Is...>) noexcept {
//    return {};
//}
//
//template <std::size_t N, std::size_t... Is>
//constexpr auto drop_first_idx(std::index_sequence<Is...>) noexcept {
//    static_assert(N <= sizeof...(Is), "Dropping too many elements");
//    return drop_first_idx_impl<N>(
//        std::make_index_sequence<sizeof...(Is) - N>{});
//}

//template <std::size_t N, std::size_t... Is>
//constexpr auto drop_last_idx(std::index_sequence<Is...>) noexcept {
//    static_assert(N <= sizeof...(Is), "Dropping too many elements");
//    return std::make_index_sequence<sizeof...(Is) - N>{};
//}
//
//template <std::size_t... Is>
//constexpr auto reverse_impl(std::index_sequence<Is...>) noexcept
//    -> std::index_sequence<(sizeof...(Is) - Is - 1)...> {
//    return {};
//}
//
//template <std::size_t... Is, std::size_t... Js>
//constexpr auto merge_impl(std::index_sequence<Is...>,
//                          std::index_sequence<Js...>) noexcept
//    -> std::index_sequence<Is..., Js...> {
//    return {};
//}
//
//template <std::size_t N, std::size_t... Is>
//constexpr auto rotate_impl(std::index_sequence<Is...> is) noexcept {
//    return merge_impl(drop_first_idx<N>(is),
//                      drop_last_idx<sizeof...(Is) - N>(is));
//}
} // namespace detail

//template <std::size_t N, typename Tuple>
//[[nodiscard]] constexpr auto drop_first(Tuple &&tuple) noexcept {
//    using tuple_t = detail::remove_cvref_t<Tuple>;
//    using idx_seq_t = typename tuple_view_traits<tuple_t>::idx_seq;
//    using res_idx_seq_t = decltype(detail::drop_first_idx<N>(idx_seq_t{}));
//    return tuple_view<Tuple, res_idx_seq_t>{std::forward<Tuple>(tuple)};
//}

//template <std::size_t N, typename Tuple>
//[[nodiscard]] constexpr auto drop_last(Tuple &&tuple) noexcept {
//    using tuple_t = detail::remove_cvref_t<Tuple>;
//    using idx_seq_t = typename tuple_view_traits<tuple_t>::idx_seq;
//    using res_idx_seq_t = decltype(detail::drop_last_idx<N>(idx_seq_t{}));
//    return tuple_view<Tuple, res_idx_seq_t>{std::forward<Tuple>(tuple)};
//}
//
//template <typename Tuple>
//[[nodiscard]] constexpr auto reverse(Tuple &&tuple) noexcept {
//    using tuple_t = detail::remove_cvref_t<Tuple>;
//    using idx_seq_t = typename tuple_view_traits<tuple_t>::idx_seq;
//    using res_idx_seq_t = decltype(detail::reverse_impl(idx_seq_t{}));
//    return tuple_view<Tuple, res_idx_seq_t>{std::forward<Tuple>(tuple)};
//}
//
//template <std::size_t N, typename Tuple>
//[[nodiscard]] constexpr auto rotate(Tuple &&tuple) noexcept {
//    using tuple_t = detail::remove_cvref_t<Tuple>;
//    using idx_seq_t = typename tuple_view_traits<tuple_t>::idx_seq;
//    using res_idx_seq_t = decltype(detail::rotate_impl<N>(idx_seq_t{}));
//    return tuple_view<Tuple, res_idx_seq_t>{std::forward<Tuple>(tuple)};
//}
} // namespace tr
