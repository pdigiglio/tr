#pragma once

#include "./fwd/at.h"
#include "./fwd/indices_for.h"
#include "./fwd/length.h"
#include "./fwd/tuple.h"

#include "./detail/ebo.h"
#include "./detail/flat_array.h"
#include "./detail/tuple_traits_utils.h"
#include "./detail/type_traits.h"
#include "./is_valid.h"
#include "./tuple_protocol.h"
#include "./type_identity.h"
#include "./type_pack.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tr {

namespace detail {

template <typename T, typename U, typename = void>
struct assign_impl {
    static auto apply(T, U) = delete;
};

template <typename T, typename U>
struct assign_impl<T, U, std::enable_if_t<std::is_assignable_v<T, U>>> {

#define ASSIGN_EXPR(lhs, rhs)                                                  \
    static_cast<decltype(lhs)>(lhs) = static_cast<decltype(rhs)>(rhs)

    constexpr static auto apply(T t, U u) noexcept(noexcept(ASSIGN_EXPR(t, u)))
        -> decltype(ASSIGN_EXPR(t, u)) {
        return (ASSIGN_EXPR(t, u));
    }

#undef ASSIGN_EXPR
};

template <typename T, typename U, int N>
struct assign_impl<T (&)[N], U> {

#define ASSIGN_EXPR(lhs, rhs)                                                  \
    flat_array_assign(static_cast<decltype(lhs)>(lhs),                         \
                      static_cast<decltype(rhs)>(rhs))

    constexpr static auto apply(T (&t)[N],
                                U u) noexcept(noexcept(ASSIGN_EXPR(t, u)))
        -> decltype(ASSIGN_EXPR(t, u)) {
        return ASSIGN_EXPR(t, u);
    }

#undef ASSIGN_EXPR
};

struct assign_t {

#define ASSIGN_EXPR(lhs, rhs)                                                  \
    assign_impl<decltype(lhs), decltype(rhs)>::apply(                          \
        static_cast<decltype(lhs)>(lhs), static_cast<decltype(rhs)>(rhs))

    template <typename T, typename U>
    constexpr auto operator()(T &&t, U &&u) const
        noexcept(noexcept(ASSIGN_EXPR(t, u))) -> decltype(ASSIGN_EXPR(t, u)) {
        return ASSIGN_EXPR(t, u);
    }

#undef ASSIGN_EXPR
};

static constexpr assign_t assign{};

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The wrapped type.
/// @tparam I An index type.
template <typename T, std::size_t I, typename>
struct tup_elem_base : ebo<T, tuple_tag<I>> {};

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The wrapped type.
/// @tparam I An index type.
template <typename T, std::size_t I>
struct tup_elem_base<T &, I, std::enable_if_t<std::is_copy_assignable_v<T>>>
    : ebo<T &, tuple_tag<I>> {

    constexpr auto operator=(tup_elem_base const &other) noexcept(
        std::is_nothrow_copy_assignable_v<T>) -> tup_elem_base & {
        this->value() = other->value();
        return *this;
    }
};

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The wrapped type.
/// @tparam I An index type.
template <typename T, std::size_t N, std::size_t I>
struct tup_elem_base<T (&)[N], I,
                     std::void_t<decltype(assign(std::declval<T (&)[N]>(),
                                                 std::declval<T (&)[N]>()))>>
    : ebo<T (&)[N], tuple_tag<I>> {

    constexpr auto operator=(tup_elem_base const &other) -> tup_elem_base & {
        assign(this->value(), other.value());
        return *this;
    }
};

/// @brief An empty-base-optimized class that adds special behavior to `ebo`
/// when the wrapped type is a built-in array.
/// @tparam T The array value type.
/// @tparam N The array size.
/// @tparam I An index type.
template <typename T, std::size_t N, std::size_t I>
struct tup_elem_base<T[N], I> : ebo<T[N], tuple_tag<I>> {

    /// @brief Default constructor.
    constexpr tup_elem_base() = default;

    /// @brief Construct from a l-value reference to a constant array.
    /// @param arr The array to contruct from.
    /// @tparam M The size of the input array (may be less or equal than `N`).
    template <std::size_t M, std::enable_if_t<(M <= N), bool> = true>
    constexpr tup_elem_base(T const (&arr)[M])
        : tup_elem_base(arr, flat_sequence_for_array(arr)) {}

    /// @brief Construct from a r-value reference to an array.
    /// @param arr The array to contruct from.
    /// @tparam M The size of the input array (may be less or equal than `N`).
    template <std::size_t M, std::enable_if_t<(M <= N), bool> = true>
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
  private:
    using base_t = tup_elem_base<T, I>;

    // NOTE: I check if `tup_elem_base.value()` is assignable here. If I check
    // whether `tup_elem.value()` is assignable, Clang will complain that
    // `tup_elem` is not yet a complete type.
#define ASSIGN_EXPR(OTHER)                                                     \
    assign(std::declval<base_t &>().value(), std::declval<OTHER>().value())

    template <typename Other>
    using assign_expr_t = decltype(ASSIGN_EXPR(Other));

    template <typename Other>
    static constexpr bool nothrow_assignable_from_v{
        noexcept(ASSIGN_EXPR(Other))};

#undef ASSIGN_EXPR

    template <typename Other>
    static constexpr bool assignable_from_v{
        is_valid_type_expr_v<assign_expr_t, Other>};

    template <typename Other>
    static constexpr bool swappable_with_v{
        std::is_swappable_with_v<decltype(std::declval<base_t &>().value()),
                                 decltype(std::declval<Other>().value())>};

    template <typename Other>
    static constexpr bool nothrow_swappable_with_v{
        std::is_nothrow_swappable_with_v<
            decltype(std::declval<base_t &>().value()),
            decltype(std::declval<Other>().value())>};

  public:
    /// @brief Copy assignment operator (when `U != T`).
    /// @param other The element to copy-assign from.
    /// @tparam U The wrapped by `other`.
    /// @returns A reference to `*this`.
    template <typename U>
    auto operator=(tup_elem<U, I> const &other) noexcept(
        tup_elem::nothrow_assignable_from_v<decltype(other)>)
        -> std::enable_if_t<tup_elem::assignable_from_v<decltype(other)>,
                            tup_elem &> {
        assign(this->value(), other.value());
        return *this;
    }

    /// @brief Move assignment operator (when `U != T`).
    /// @param other The element to move-assign from.
    /// @tparam U The wrapped by `other`.
    /// @returns A reference to `*this`.
    template <typename U>
    auto operator=(tup_elem<U, I> &&other) noexcept(
        tup_elem::nothrow_assignable_from_v<decltype(other)>)
        -> std::enable_if_t<tup_elem::assignable_from_v<decltype(other)>,
                            tup_elem &> {
        assign(this->value(), std::move(other).value());
        return *this;
    }

    /// @brief Swap two instances of `tup_elem`.
    /// @param lhs The left-hand side instance.
    /// @param rhs The right-hand side instance.
    template <typename U>
    constexpr auto swap(tup_elem<U, I> &rhs) noexcept(
        tup_elem::nothrow_swappable_with_v<decltype(rhs)>)
        -> std::enable_if_t<tup_elem::swappable_with_v<decltype(rhs)>> {
        using std::swap;
        swap(this->value(), rhs.value());
    }

    template <typename Int>
    [[nodiscard]] constexpr auto
    operator[](std::integral_constant<Int, I>) const &noexcept
        //-> decltype(std::declval<base_t const &>().value()) {
        -> decltype(auto) {
        return this->value();
    }

    template <typename Int>
    [[nodiscard]] constexpr auto
    operator[](std::integral_constant<Int, I>) &noexcept
        //-> decltype(std::declval<base_t &>().value()) {
        -> decltype(auto) {
        return this->value();
    }

    template <typename Int>
    [[nodiscard]] constexpr auto
    operator[](std::integral_constant<Int, I>) const &&noexcept
        //-> decltype(std::declval<base_t const &&>().value()) {
        -> decltype(auto) {
        return std::move(*this).value();
    }

    template <typename Int>
    [[nodiscard]] constexpr auto
    operator[](std::integral_constant<Int, I>) &&noexcept
        //-> decltype(std::declval<base_t &&>().value()) {
        -> decltype(auto) {
        return std::move(*this).value();
    }

    /// @brief An undefined member function to query the type of this
    /// tup_elem in traits like std::tuple_element.
    ///
    /// @return `type_identity<T>`. If I return `T` directly, I can't
    /// use this technique to query the type of a `tup_elem<T[N]>`, as
    /// I can't return arrays.
    static type_identity<T>
        get_type(std::integral_constant<std::size_t, I>) /* undefined */;
};

template <typename... Ts, std::size_t... Is>
struct tuple_base<type_pack<Ts...>, std::index_sequence<Is...>>
    : tup_elem<Ts, Is>... {

  private:
    using index_sequence_t = std::index_sequence<Is...>;
    using type_pack_t = type_pack<Ts...>;

    // Check if all of the base `tup_elem` can be assigned.
#define ASSIGN_EXPR(OTHER)                                                     \
    ((std::declval<tup_elem<Ts, Is> &>() = std::declval<OTHER>()), ...)

    template <typename Other>
    using assign_expr_t = decltype(ASSIGN_EXPR(Other));

    template <typename Other>
    static constexpr bool nothrow_assignable_from_v{
        noexcept(ASSIGN_EXPR(Other))};

#undef ASSIGN_EXPR

    template <typename Other>
    static constexpr bool assignable_from_v{
        is_valid_type_expr_v<assign_expr_t, Other>};

#define SWAP_EXPR(OTHER)                                                       \
    ((std::declval<tup_elem<Ts, Is> &>().swap(std::declval<OTHER>())), ...)

    template <typename Other>
    using swap_expr_t = decltype(SWAP_EXPR(Other));

    template <typename Other>
    static constexpr bool nothrow_swappable_with_v{noexcept(SWAP_EXPR(Other))};

#undef SWAP_EXPR

    template <typename Other>
    static constexpr bool swappable_with_v{
        is_valid_type_expr_v<swap_expr_t, Other>};

  public:
    using tup_elem<Ts, Is>::operator[]...;
    using tup_elem<Ts, Is>::get_type...;

    /// @brief Copy assignment operator (enabled if there's a mismatch between
    /// the `Us...` and the `Ts...`).
    ///
    /// @details This copy-assignment operator is enabled if each of the
    /// `Ts...` is copy assignable from the corresponding type in `Us...`.
    ///
    /// @param other The element to copy-assign from.
    /// @tparam Us... The types wrapped by `other`.
    /// @returns A reference to `*this`.
    template <typename... Us>
    auto operator=(
        tuple_base<type_pack<Us...>, index_sequence_t> const
            &other) noexcept(tuple_base::
                                 nothrow_assignable_from_v<decltype(other)>)
        -> std::enable_if_t<tuple_base::assignable_from_v<decltype(other)>,
                            tuple_base &> {
        (
            [&] {
                using lhs_t = tup_elem<Ts, Is>;
                using rhs_t = tup_elem<Us, Is>;
                static_cast<lhs_t &>(*this) = static_cast<rhs_t &>(other);
            }(),
            ...);
        return *this;
    }

    /// @brief Move assignment operator (enabled if there's a mismatch between
    /// the `Us...` and the `Ts...`).
    ///
    /// @details This move-assignment operator is enabled if each of the
    /// `Ts...` is move assignable from the corresponding type in `Us...`.
    ///
    /// @param other The element to copy-assign from.
    /// @tparam Us... The types wrapped by `other`.
    /// @returns A reference to `*this`.
    template <typename... Us>
    auto
    operator=(tuple_base<type_pack<Us...>, index_sequence_t> &&other) noexcept(
        tuple_base::nothrow_assignable_from_v<decltype(other)>)
        -> std::enable_if_t<tuple_base::assignable_from_v<decltype(other)>,
                            tuple_base &> {
        (
            [&] {
                using lhs_t = tup_elem<Ts, Is>;
                using rhs_t = tup_elem<Us, Is>;
                static_cast<lhs_t &>(*this) = static_cast<rhs_t &&>(other);
            }(),
            ...);
        return *this;
    }

    template <typename... Us>
    constexpr auto
    swap(tuple_base<type_pack<Us...>, index_sequence_t> &rhs) noexcept(
        tuple_base::nothrow_swappable_with_v<decltype(rhs)>)
        -> std::enable_if_t<tuple_base::swappable_with_v<decltype(rhs)>> {
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

/// @brief A fixed-size collection of (possibly) hererogeneous values. 
/// @tparam ...Ts The types stored by the tuple (can be empty).
template <typename... Ts>
struct tuple
    : detail::tuple_base<type_pack<Ts...>, std::index_sequence_for<Ts...>> {
  private:
    using index_sequence_t = std::index_sequence_for<Ts...>;
    using type_pack_t = type_pack<Ts...>;
    using base_t = detail::tuple_base<type_pack_t, index_sequence_t>;

    template <typename U, typename Tuple, std::size_t I>
    static constexpr bool is_constructible_v{
        std::is_constructible_v<U &, decltype(std::declval<Tuple>()[zuic<I>])>};

    template <typename U, typename Tuple, std::size_t I>
    static constexpr bool is_nothrow_constructible_v{
        std::is_nothrow_constructible_v<
            U &, decltype(std::declval<Tuple>()[zuic<I>])>};

    template <typename Tuple, typename... Us, std::size_t... Is>
    [[nodiscard]] static auto
    cast_impl(Tuple &&t, type_pack<Us...>, std::index_sequence<Is...>) noexcept(
        (tuple::is_nothrow_constructible_v<Us, Tuple, Is> && ...))
        //-> std::enable_if_t<(tuple::is_constructible_v<Us, Tuple, Is> && ...),
        //                    tuple<Us...>> {
        -> tuple<Us...> {
        return {static_cast<Tuple &&>(t)[zuic<Is>]...};
    }

  public:
    using base_t::operator=;

    /// @brief Implicit conversion function.
    ///
    /// @details As `tr::tuple<Ts...>` can only be aggregate-initialized from
    /// instances of `Ts...`, I provide an implicit conversion function to
    /// allow `std::tuple`-like conversions:
    ///
    /// @code
    /// int i{};
    /// std::tuple<int&> sti_ref{i};
    /// std::tuple<int>  sti{stri_ref}; // ok
    ///
    /// tr::tuple<int&> ti_ref{i};
    /// //tr::tuple<int>  ti{stri_ref}; // doesn't compile
    /// tr::tuple<int>  ti = stri_ref;  // ok
    /// @endcode
    ///
    /// @tparam ...Us The types stored by the target tuple.
    ///
    /// @return A `tr::tuple<Us...>` constructed from `*this`.
    template <typename... Us>
    //, typename = decltype(tuple::cast_impl( std::declval<tuple const &>(),
    //type_pack<Us...>{}, index_sequence_t{}))>
    [[nodiscard]] operator tuple<Us...>() const & {
        return tuple::cast_impl(*this, type_pack<Us...>{}, index_sequence_t{});
    }

    template <typename... Us>
    //, typename = decltype(tuple::cast_impl( std::declval<tuple const &>(),
    //type_pack<Us...>{}, index_sequence_t{}))>
    [[nodiscard]] operator tuple<Us...>() & {
        return tuple::cast_impl(*this, type_pack<Us...>{}, index_sequence_t{});
    }

    template <typename... Us>
    //, typename = decltype(tuple::cast_impl( std::declval<tuple const &>(),
    //type_pack<Us...>{}, index_sequence_t{}))>
    [[nodiscard]] operator tuple<Us...>() const && {
        return tuple::cast_impl(std::move(*this), type_pack<Us...>{},
                                index_sequence_t{});
    }

    template <typename... Us>
    //, typename = decltype(tuple::cast_impl( std::declval<tuple const &>(),
    //type_pack<Us...>{}, index_sequence_t{}))>
    [[nodiscard]] operator tuple<Us...>() && {
        return tuple::cast_impl(std::move(*this), type_pack<Us...>{},
                                index_sequence_t{});
    }
};

template <typename... Ts>
tuple(Ts const &...) -> tuple<Ts...>;

namespace detail {

/// @brief An alias template to check the validity of the `swap` expression for
/// a `tr::tuple`.
/// @tparam Lhs The type of the left-hand-side `tr::tuple`.
/// @tparam Rhs The type of the right-hand-side `tr::tuple`.
template <typename Lhs, typename Rhs>
using tuple_swap_expr_t =
    decltype(std::declval<Lhs>().swap(std::declval<Rhs>()));

/// @brief A variable template which holds `true`, if two `tr::tuple`s can be
/// swapped, and `false` otherwise.
/// @tparam Lhs The type of the left-hand-side `tr::tuple`.
/// @tparam Rhs The type of the right-hand-side `tr::tuple`.
template <typename Lhs, typename Rhs>
static constexpr bool tuple_swappable_with_v{
    is_valid_type_expr_v<tuple_swap_expr_t, Lhs, Rhs>};

} // namespace detail

/// @brief Swap two tuples of different type (e.g. `tuple<int>` and
/// `tuple<int&>`).
///
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename... Ts, typename U, typename... Us>
constexpr auto swap(tuple<Ts...> &lhs,
                    tuple<U, Us...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<type_pack_c<Ts...> != type_pack_c<U, Us...>,
                        decltype(lhs.swap(rhs))> {
    lhs.swap(rhs);
}

/// @brief Swap two tuples of the same type.
///
/// @param lhs The left-hand-side tuple.
/// @param rhs The right-hand-side tuple.
template <typename... Us>
constexpr auto swap(tuple<Us...> &lhs,
                    tuple<Us...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> std::enable_if_t<
        detail::tuple_swappable_with_v<decltype(lhs), decltype(rhs)>> {
    lhs.swap(rhs);
}

/// @brief Swap two tuples.
/// @details This overload is deleted to make sure `tr::tuple` is not
/// swappable if any of its contained element is not.
template <typename... Us>
auto swap(tuple<Us...> &lhs, tuple<Us...> &rhs) -> std::enable_if_t<
    !detail::tuple_swappable_with_v<decltype(lhs), decltype(rhs)>> = delete;

/// @brief Creates a `tuple` of l-value references to its arguments. This is
/// analogous to `std::tie`.
/// @param ...args Any number of l-value arguments to construct the `tuple`.
/// @return A `tuple` of l-value references.
template <typename... Ts>
[[nodiscard]] constexpr tuple<Ts &...> tie(Ts &...args) noexcept {
    return {args...};
}

/// @brief Creates a `tuple` of l- or r-value references to its l- or r-value
/// arguments. This is analogous to `std::forward_as_tuple`.
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
