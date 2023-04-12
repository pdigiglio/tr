#pragma once

#include "./fwd/at.h"
#include "./fwd/indices_for.h"
#include "./fwd/length.h"
#include "./fwd/tuple.h"

#include "./detail/ebo.h"
#include "./detail/flat_array.h"
#include "./detail/tuple_traits_utils.h"
#include "./detail/type_traits.h"
#include "./tuple_protocol.h"
#include "./type_identity.h"
#include "./type_pack.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tr {

namespace detail {

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The wrapped type.
/// @tparam I An index type.
template <typename T, std::size_t I>
struct tup_elem_base : ebo<T, tuple_tag<I>> {};

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The array value type.
/// @tparam B The array size.
/// @tparam I An index type.
template <typename T, std::size_t N, std::size_t I>
struct tup_elem_base<T[N], I> : ebo<T[N], tuple_tag<I>> {

    /// @brief Default constructor.
    constexpr tup_elem_base() noexcept : ebo<T[N], tuple_tag<I>>{} {}
    //constexpr tup_elem_base() noexcept = default;

    /// @brief Construct from a l-value reference to a constant array.
    /// @param arr The array to contruct from.
    /// @tparam M The size of the input array (may be less or equal than `N`).
    template <std::size_t M>
    constexpr tup_elem_base(T const (&arr)[M])
        : tup_elem_base(arr, flat_sequence_for_array(arr)) {}

    /// @brief Construct from a r-value reference to an array.
    /// @param arr The array to contruct from.
    /// @tparam M The size of the input array (may be less or equal than `N`).
    template <std::size_t M>
    constexpr tup_elem_base(T(&&arr)[M])
        : tup_elem_base(std::move(arr), flat_sequence_for_array(arr)) {}

  private:
    template <typename Arr, std::size_t... Is>
    constexpr tup_elem_base(Arr &&arr, std::index_sequence<Is...>)
        : ebo<T[N], tuple_tag<I>>{
              flat_array_at<Is>(std::forward<Arr>(arr))...} {}
};

template <typename T, std::size_t I>
struct tup_elem : tup_elem_base<T, I> {

    template <typename Int>
    constexpr T const &
    operator[](std::integral_constant<Int, I> ic) const &noexcept {
        return tup_elem::subscript_impl(*this, ic);
    }

    template <typename Int>
    constexpr T &operator[](std::integral_constant<Int, I> ic) &noexcept {
        return tup_elem::subscript_impl(*this, ic);
    }

    template <typename Int>
    constexpr T const &&
    operator[](std::integral_constant<Int, I> ic) const &&noexcept {
        return tup_elem::subscript_impl(std::move(*this), ic);
    }

    template <typename Int>
    constexpr T &&operator[](std::integral_constant<Int, I> ic) &&noexcept {
        return tup_elem::subscript_impl(std::move(*this), ic);
    }

    /// @brief Swap two instances of `tup_elem`.
    /// @param lhs The left-hand side instance.
    /// @param rhs The right-hand side instance.
    template <typename U>
    constexpr auto swap(tup_elem<U, I> &rhs) noexcept(
        are_tuples_nothrow_swappable_v<tup_elem, tup_elem<U, I>>)
        -> std::enable_if_t<are_tuples_swappable_v<tup_elem, tup_elem<U, I>>> {
        using std::swap;
        swap((*this)[zuic<I>], rhs[zuic<I>]);
    }

    /// @brief An undefined member function to query the type of this
    /// tup_elem in traits like std::tuple_element.
    ///
    /// @return `type_identity<T>`. If I return `T` directly, I can't
    /// use this technique to query the type of a `tup_elem<T[N]>`, as
    /// I can't return arrays.
    static type_identity<T>
        get_type(std::integral_constant<std::size_t, I>) /* undefined */;

  private:
    template <typename TupElem, typename Int,
              typename = std::enable_if_t<std::is_integral_v<Int>>>
    constexpr static decltype(auto)
    subscript_impl(TupElem &&tupElem, std::integral_constant<Int, I>) noexcept {
        using ebo_t = ebo<T, tuple_tag<I>>;
        return get_ebo_val(forward_as_base<ebo_t, TupElem>(tupElem));
    }
};

template <typename... Ts, std::size_t... Is>
struct tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>
    : tup_elem<Ts, Is>... {
    using index_sequence_t = std::index_sequence<Is...>;
    using type_pack_t = type_pack<Ts...>;

    using tup_elem<Ts, Is>::operator[]...;
    using tup_elem<Ts, Is>::get_type...;

    template <typename... Us>
    constexpr auto
    swap(tuple_base<type_pack<Us...>, index_sequence_t> &rhs) noexcept(
        are_tuples_nothrow_swappable_v<
            tuple_base, tuple_base<type_pack<Us...>, index_sequence_t>>)
        -> std::enable_if_t<are_tuples_swappable_v<
            tuple_base, tuple_base<type_pack<Us...>, index_sequence_t>>> {
        (
            [&] {
                using lhs_t = tup_elem<Ts, Is>;
                using rhs_t = tup_elem<Us, Is>;
                static_cast<lhs_t &>(*this).swap(static_cast<rhs_t &>(rhs));
            }(),
            ...);
    }
};
} // namespace detail

template <typename... Ts>
struct tuple
    : detail::tuple_base<type_pack<Ts...>, std::index_sequence_for<Ts...>> {};

template <typename... Ts>
tuple(Ts const &...) -> tuple<Ts...>;

/// @brief Swap two tuples.
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename T, typename... Ts, typename U, typename... Us>
constexpr auto swap(tuple<T, Ts...> &lhs,
                    tuple<U, Us...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<type_pack_c<T, Ts...> != type_pack_c<U, Us...>,
                        decltype(lhs.swap(rhs))> {
    lhs.swap(rhs);
}

/// @brief Swap two tuples.
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename... Ts>
constexpr auto swap(tuple<Ts...> &lhs,
                    tuple<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<detail::is_tuple_swappable_v<tuple<Ts...>>> {
    lhs.swap(rhs);
}

/// @brief Swap two tuples. Deleted overload to make sure `tr::tuple` is not
/// swappable if any of its contained element is not.
template <typename... Ts>
auto swap(tuple<Ts...> &, tuple<Ts...> &)
    -> std::enable_if_t<!detail::is_tuple_swappable_v<tuple<Ts...>>> = delete;

/// @brief Creates a `tuple` of l-value references to its arguments. This is
/// analogous to `std::tie`.
/// @param ...args Any number of l-value arguments to construct the `tuple`.
/// @return A `tuple` of l-value references.
template <typename... Ts>
[[nodiscard]] constexpr tuple<Ts &...> tie(Ts &...args) noexcept {
    return {args...};
}

/// @brief Creates a `tuple` of l- or r-value references to its l- or r-value
/// arguments. This is analogous to `std::tie`.
/// @param ...args Any number of arguments.
/// @return A `tuple` of l- or r-value references.
template <typename... Ts>
[[nodiscard]] constexpr tuple<Ts &&...>
forward_as_tuple(Ts &&...args) noexcept {
    return {std::forward<Ts>(args)...};
}

// -- Tuple protocol
template <typename... Ts>
struct tup_size<tuple<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};
// --

namespace detail {
template <std::size_t I, typename Tuple>
constexpr decltype(auto) tuple_get(Tuple &&t) noexcept {
    using tuple_t = remove_cvref_t<Tuple>;
    static_assert(I < tup_size_v<tuple_t>, "Index out of range");
    return std::forward<Tuple>(t)[zuic<I>];
}
} // namespace detail

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...> const &t) noexcept {
    return detail::tuple_get<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...> &t) noexcept {
    return detail::tuple_get<I>(t);
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...> &&t) noexcept {
    return detail::tuple_get<I>(std::move(t));
}

template <typename... Ts>
struct at_impl<tuple<Ts...>> {
    template <typename Iterable, typename Idx>
    static constexpr decltype(auto) apply(Iterable &&tuple, Idx) noexcept {
        using idx_t = std::integral_constant<std::size_t, Idx::value>;
        return std::forward<Iterable>(tuple)[idx_t{}];
    }
};

template <typename... Ts>
struct length_impl<tuple<Ts...>> {
    template <typename Iterable>
    static constexpr auto apply(Iterable &&) noexcept
        -> value_constant<sizeof...(Ts)> {
        return {};
    }
};

//template <typename... Ts>
//struct indices_for_impl<tuple<Ts...>> {
//
//    template <typename Sized>
//    [[nodiscard]] static constexpr auto apply(Sized &&) noexcept ->
//        typename tuple<Ts...>::index_sequence_t {
//        return {};
//    }
//};

} // namespace tr

namespace std {
// Specialize
//
//  * std::tuple_size
//  * std::tuple_element
//
// to allow structured bindings to tr::tuple

template <typename... Ts>
struct tuple_size<::tr::tuple<Ts...>>
    : integral_constant<size_t, sizeof...(Ts)> {};

template <size_t I, typename... Ts>
struct tuple_element<I, ::tr::tuple<Ts...>>
    : decltype(::tr::tuple<Ts...>::get_type(::tr::zuic<I>)) {};
} // namespace std
