#pragma once

#include "./fwd/at.h"
#include "./fwd/length.h"

#include "./detail/ebo.h"
#include "./detail/type_pack.h"
#include "./detail/type_traits.h"
#include "./tuple_protocol.h"
#include "./type_identity.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tr {
template <typename T>
using as_array = std::enable_if_t<std::is_array_v<T>, T>;

namespace detail {

/// @brief Tag type for tuple elements.
template <std::size_t>
struct tuple_tag;

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The wrapped type.
/// @tparam I An index type.
template <typename T, std::size_t I>
struct tup_elem_base : ebo<T, tuple_tag<I>> {};

template <typename T, std::size_t N, std::size_t... Is>
[[nodiscard]] constexpr auto
flat_array_extent_impl(T const (&)[N], std::index_sequence<Is...>) noexcept {
    return std::integral_constant<std::size_t,
                                  (std::extent_v<T[N], Is> * ...)>{};
}

template <typename T, std::size_t N>
[[nodiscard]] constexpr auto flat_array_extent(T const (&arr)[N]) noexcept {
    constexpr auto rank = std::rank_v<T[N]>;
    return flat_array_extent_impl(arr, std::make_index_sequence<rank>{});
}

template <typename T, std::size_t N>
[[nodiscard]] constexpr auto
flat_sequence_for_array(T const (&arr)[N]) noexcept {
    auto flatExtent = flat_array_extent(arr);
    return std::make_index_sequence<flatExtent>{};
}

template <std::size_t I, typename Arr>
[[nodiscard]] constexpr decltype(auto) flat_array_at_impl(Arr &&arr) noexcept {
    using array_t = remove_cvref_t<Arr>;
    static_assert(
        std::is_array_v<array_t>,
        "Arr is not an array, nor a cv-ref qualified reference to one");

    constexpr auto extent = std::extent_v<array_t>;
    constexpr auto rank = std::rank_v<array_t>;
    if constexpr (rank == 1) {
        static_assert(I < extent, "Index out of bounds");
        return forward_like<Arr>(arr[I]);
    } else {
        using elem_extent_t = decltype(flat_array_extent(arr[0]));
        constexpr auto row = I / elem_extent_t::value;
        constexpr auto rest = I % elem_extent_t::value;

        static_assert(row < extent, "Index out of bounds");
        return flat_array_at_impl<rest>(arr[row]);
    }
}

template <std::size_t I, typename T, std::size_t N>
[[nodiscard]] constexpr decltype(auto)
flat_array_at(T const (&arr)[N]) noexcept {
    return flat_array_at_impl<I>(arr);
}

template <std::size_t I, typename T, std::size_t N>
[[nodiscard]] constexpr decltype(auto) flat_array_at(T(&&arr)[N]) noexcept {
    return flat_array_at_impl<I>(std::move(arr));
}

template <typename T, std::size_t N, std::size_t I>
struct tup_elem_base<T[N], I> : ebo<T[N], tuple_tag<I>> {
    constexpr tup_elem_base() noexcept : ebo<T[N], tuple_tag<I>>{} {}

    template <std::size_t M>
    constexpr tup_elem_base(T const (&arr)[M])
        : tup_elem_base(arr, flat_sequence_for_array(arr)) {}

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
struct tup_elem;

template <typename, typename>
struct tuple_swap_trait;

template <typename T, typename U, std::size_t I>
struct tuple_swap_trait<tup_elem<T, I>, tup_elem<U, I>> {
    static constexpr bool swappable{
        std::is_swappable_with_v<std::add_lvalue_reference_t<T>,
                                 std::add_lvalue_reference_t<U>>};

    static constexpr bool nothrow_swappable{
        std::is_nothrow_swappable_with_v<std::add_lvalue_reference_t<T>,
                                         std::add_lvalue_reference_t<U>>};
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
        tuple_swap_trait<tup_elem, tup_elem<U, I>>::nothrow_swappable)
        -> std::enable_if_t<
            tuple_swap_trait<tup_elem, tup_elem<U, I>>::swappable> {
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
        return get_ebo_val(forward_as<ebo_t, TupElem>(tupElem));
    }
};

template <typename TL, typename IL>
struct tuple_base;

template <typename... Ts, typename... Us, std::size_t... Is>
struct tuple_swap_trait<
    tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>,
    tuple_base<type_pack<Us...>, std::index_sequence<Is...>>> {

    static constexpr bool swappable{
        (tuple_swap_trait<tup_elem<Ts, Is>, tup_elem<Us, Is>>::swappable &&
         ... && true)};

    static constexpr bool nothrow_swappable{
        (tuple_swap_trait<tup_elem<Ts, Is>,
                          tup_elem<Us, Is>>::nothrow_swappable &&
         ... && true)};
};

template <typename... Ts, std::size_t... Is>
struct tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>
    : tup_elem<Ts, Is>... {
    using tup_elem<Ts, Is>::operator[]...;
    using tup_elem<Ts, Is>::get_type...;

    template <typename... Us>
    constexpr auto
    swap(tuple_base<type_pack<Us...>, std::index_sequence<Is...>>
             &rhs) noexcept(tuple_swap_trait<tuple_base,
                                             tuple_base<
                                                 type_pack<Us...>,
                                                 std::index_sequence<Is...>>>::
                                nothrow_swappable)

        -> std::enable_if_t<tuple_swap_trait<
            tuple_base, tuple_base<type_pack<Us...>,
                                   std::index_sequence<Is...>>>::swappable> {
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
struct tuple : detail::tuple_base<detail::type_pack<Ts...>,
                                  std::index_sequence_for<Ts...>> {};

template <typename... Ts>
tuple(Ts const &...) -> tuple<Ts...>;

/// @brief Swap two tuples.
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename T, typename... Ts, typename U, typename... Us,
          typename = std::enable_if_t<((type_c<T> != type_c<U>) || ... ||
                                       (type_c<Ts> != type_c<Us>))>>
constexpr auto swap(tuple<T, Ts...> &lhs, tuple<U, Us...> &rhs)
    -> decltype(lhs.swap(rhs)) {
    lhs.swap(rhs);
}

/// @brief Swap two tuples.
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename... Ts>
constexpr auto swap(tuple<Ts...> &lhs,
                    tuple<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<
        detail::tuple_swap_trait<
            detail::tuple_base<detail::type_pack<Ts...>,
                               std::index_sequence_for<Ts...>>,
            detail::tuple_base<detail::type_pack<Ts...>,
                               std::index_sequence_for<Ts...>>>::swappable,
        decltype(lhs.swap(rhs))> {
    lhs.swap(rhs);
}

/// @brief Swap two tuples. Deleted overload to make sure `tr::tuple` is not
/// swappable if any of its contained element is not.
template <typename... Ts>
auto swap(tuple<Ts...> &, tuple<Ts...> &)
    -> std::enable_if_t<!detail::tuple_swap_trait<
        detail::tuple_base<detail::type_pack<Ts...>,
                           std::index_sequence_for<Ts...>>,
        detail::tuple_base<detail::type_pack<Ts...>,
                           std::index_sequence_for<Ts...>>>::swappable> =
    delete;

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
        return std::forward<Iterable>(tuple)[Idx{}];
    }
};

template <typename... Ts>
struct length_impl<tuple<Ts...>> {
    template <typename Iterable>
    static constexpr value_constant<sizeof...(Ts)> apply(Iterable &&) noexcept {
        return {};
    }
};

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
