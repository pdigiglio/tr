#pragma once

#include "./detail/type_traits.h"
#include "./tuple_protocol.h"

#include <cstddef>
#include <utility>

namespace tr {
// https://www.foonathan.net/2020/05/fold-tricks/
namespace detail {
template <typename Tuple, typename Predicate, std::size_t... Is>
constexpr bool any_of(Tuple &&tuple, Predicate pred,
                      std::index_sequence<Is...>) {
    using tr::get;
    return (pred(get<Is>(std::forward<Tuple>(tuple))) || ...);
}

template <typename Tuple, typename Predicate, std::size_t... Is>
constexpr std::ptrdiff_t count_if(Tuple &&tuple, Predicate pred,
                                  std::index_sequence<Is...>) {
    using tr::get;
    return (static_cast<std::ptrdiff_t>(0) + ... +
            pred(get<Is>(std::forward<Tuple>(tuple))));
}

template <typename Tuple, typename Predicate, std::size_t... Is>
constexpr std::ptrdiff_t find_if(Tuple &&tuple, Predicate pred,
                                 std::index_sequence<Is...>) {
    using tr::get;

    bool stop{};
    std::ptrdiff_t idx{};
    (void)((stop = pred(get<Is>(std::forward<Tuple>(tuple))), idx = Is, stop) ||
           ...);
    return stop ? idx : -1;
    //                  ^ -1 if predicate not satisfied as it's not
    //                  easy to get the size of a tuple.

    // return idx + !stop;
    //  If querying the size of a tuple becomes easier, this may be
    //  considered (which is also consistend with the STL choices).
}

template <typename Tuple, typename BinaryPredicate, std::size_t... Is>
constexpr std::ptrdiff_t
adjacent_find_unchecked(Tuple &&tuple, BinaryPredicate pred,
                        std::ptrdiff_t notFound, std::index_sequence<Is...>) {
    bool stop{};
    std::ptrdiff_t idx{};
    (void)((stop = pred(get<Is>(std::forward<Tuple>(tuple)),
                        get<Is + 1>(std::forward<Tuple>(tuple))),
            idx = Is, stop) ||
           ...);
    return stop ? idx : notFound;
}

template <typename Tuple0, typename Tuple1, typename Cmp, std::size_t... Is>
constexpr bool equal_unchecked(Tuple0 &&lhs, Tuple1 &&rhs, Cmp cmp,
                               std::index_sequence<Is...>) {
    using tr::get;
    return (
        cmp(get<Is>(std::forward<Is>(lhs)), get<Is>(std::forward<Is>(rhs))) &&
        ...);
}

template <typename T = void>
struct equal_pred {
    T val;

    template <typename U>
    constexpr bool operator()(U &&elem) const;
};

template <>
struct equal_pred<void> {
    template <typename T, typename U>
    constexpr bool operator()(T &&lhs, U &&rhs) const;
};

template <typename T>
template <typename U>
constexpr bool equal_pred<T>::operator()(U &&elem) const {
    return equal_pred<>{}(this->val, std::forward<U>(elem));
}

template <typename T, typename U>
constexpr bool equal_pred<void>::operator()(T &&lhs, U &&rhs) const {
    auto const check =
        detail::check_expr([](auto &&x, auto &&y) -> decltype(x == y) {});
    if constexpr (decltype(check(lhs, rhs))::value) {
        return lhs == rhs;
    } else {
        return false;
    }
}
} // namespace detail

template <typename Tuple, typename Predicate>
constexpr bool any_of(Tuple &&tuple, Predicate pred) {
    using tuple_t = std::remove_reference_t<Tuple>;
    auto const tupleSize = tr::tup_size_v<tuple_t>;
    return detail::any_of(std::forward<Tuple>(tuple), pred,
                          std::make_index_sequence<tupleSize>{});
}

template <typename Tuple, typename Predicate>
constexpr bool none_of(Tuple &&tuple, Predicate pred) {
    return !any_of(std::forward<Tuple>(tuple), pred);
}

/// @brief Returns the number the elements in `tuple` that satisfies `pred`.
/// @tparam Tuple The tuple type.
/// @tparam Predicate The predicate type.
/// @param tuple The tuple to examine.
/// @param pred The unary predicate to check for.
/// @return The number the elements in `tuple` that satisfies `pred`.
template <typename Tuple, typename Predicate>
constexpr std::ptrdiff_t count_if(Tuple &&tuple, Predicate pred) {
    using tuple_t = std::remove_reference_t<Tuple>;
    auto const tupleSize = tr::tup_size_v<tuple_t>;
    return detail::count_if(std::forward<Tuple>(tuple), pred,
                            std::make_index_sequence<tupleSize>{});
}

/// @brief Returns the number the elements in `tuple` that compares equal
/// to `val`. Note: if the expression `elem == val` is not well formed for
/// some `elem` in `tuple`, the comparison will return `false`.
///
/// @tparam Tuple The tuple type.
/// @tparam T The value type.
/// @param tuple The tuple to examine.
/// @param val The value to look for.
/// @return The number the elements in `tuple` that compares equal to `val`.
template <typename Tuple, typename T>
constexpr std::ptrdiff_t count(Tuple &&tuple, T const &val) {
    return count_if(std::forward<Tuple>(tuple),
                    detail::equal_pred<T const &>{val});
}

/// @brief Returns the index of the first element in `tuple` that satisfies
/// `pred`.
/// @tparam Tuple The tuple type.
/// @tparam Predicate The predicate type.
/// @param tuple The tuple to examine.
/// @param pred The unary predicate to check for.
/// @return The index of the first element in `tuple` that satisfies `pred`.
template <typename Tuple, typename Predicate>
constexpr std::ptrdiff_t find_if(Tuple &&tuple, Predicate pred) {
    using tuple_t = std::remove_reference_t<Tuple>;
    auto const tupleSize = tr::tup_size_v<tuple_t>;
    return detail::find_if(std::forward<Tuple>(tuple), pred,
                           std::make_index_sequence<tupleSize>{});
}

/// @brief Returns the index of the first element in `tuple` that does not
/// satisfy `pred`.
/// @tparam Tuple The tuple type.
/// @tparam Predicate The predicate type.
/// @param tuple The tuple to examine.
/// @param pred The unary predicate to check for.
/// @return The index of the first element in `tuple` that does not satisfy
/// `pred`.
template <typename Tuple, typename Predicate>
constexpr std::ptrdiff_t find_if_not(Tuple &&tuple, Predicate pred) {
    return find_if(std::forward<Tuple>(tuple), [pred](auto &&elem) {
        return !pred(std::forward<decltype(elem)>(elem));
    });
}

/// @brief Returns the index of the first element in `tuple` that compares
/// to `val`. Note: if the expression `elem == val` is not well formed for
/// some `elem` in `tuple`, the comparison will return `false`.
///
/// @tparam Tuple The tuple type.
/// @tparam T The value type.
/// @param tuple The tuple to examine.
/// @param val The value to look for.
/// @return The index of the first element in `tuple` that compares equal to
/// `val`.
template <typename Tuple, typename T>
constexpr std::ptrdiff_t find(Tuple &&tuple, T const &val) {
    return find_if(std::forward<Tuple>(tuple),
                   detail::equal_pred<T const &>{val});
}

template <typename Tuple, typename BinaryPredicate>
constexpr std::ptrdiff_t adjacent_find(Tuple &&tuple, BinaryPredicate pred) {
    using tuple_t = std::remove_reference_t<Tuple>;
    auto const tupleSize = tr::tup_size_v<tuple_t>;

    std::ptrdiff_t const notFound{-1};
    //                              ^
    //   -1 if predicate not satisfied as it's not easy to get the size of
    //   a tuple.

    if constexpr (tupleSize < 2) {
        return notFound;
    } else {
        return detail::adjacent_find_unchecked(
            std::forward<Tuple>(tuple), pred, notFound,
            std::make_index_sequence<tupleSize - 1>{});
    }
}

template <typename Tuple>
constexpr std::ptrdiff_t adjacent_find(Tuple &&tuple) {
    return adjacent_find(std::forward<Tuple>(tuple), detail::equal_pred<>{});
}

template <typename Tuple0, typename Tuple1, typename Cmp>
constexpr bool equal(Tuple0 &&lhs, Tuple1 &&rhs, Cmp cmp) {
    using tuple0_t = std::remove_reference_t<Tuple0>;
    auto const tuple0Size = tr::tup_size_v<tuple0_t>;

    using tuple1_t = std::remove_reference_t<Tuple1>;
    auto const tuple1Size = tr::tup_size_v<tuple1_t>;

    if constexpr (tuple0Size != tuple1Size) {
        return false;
    } else {
        return detail::equal_unchecked(std::forward<Tuple0>(lhs),
                                       std::forward<Tuple1>(rhs), cmp,
                                       std::make_index_sequence<tuple0Size>{});
    }
}

template <typename Tuple0, typename Tuple1>
constexpr bool equal(Tuple0 &&lhs, Tuple1 &&rhs) {
    return equal(std::forward<Tuple0>(lhs), std::forward<Tuple1>(rhs),
                 detail::equal_pred<>{});
}
} // namespace tr
