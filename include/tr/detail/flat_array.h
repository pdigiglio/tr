#pragma once

#include <cstdint>
#include <type_traits>

namespace tr {
namespace detail {

template <typename, typename>
struct same_extents : std::true_type {};

template <typename T, typename U, std::size_t N>
struct same_extents<T[N], U> : std::false_type {};

template <typename T, typename U, std::size_t N>
struct same_extents<T, U[N]> : std::false_type {};

template <typename T, typename U, std::size_t N, std::size_t M>
struct same_extents<T[N], U[M]>
    : std::bool_constant<(N == M) && same_extents<T, U>::value> {};

template <typename T, typename U>
static constexpr bool same_extents_v{same_extents<T, U>::value};

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
[[nodiscard]] constexpr decltype(auto)
flat_array_at(T (&arr)[N]) noexcept {
    return flat_array_at_impl<I>(arr);
}

template <std::size_t I, typename T, std::size_t N>
[[nodiscard]] constexpr decltype(auto) flat_array_at(T(&&arr)[N]) noexcept {
    return flat_array_at_impl<I>(std::move(arr));
}

template <typename T, typename U, std::size_t N, std::size_t... Is>
void flat_array_assign_impl(T (&to)[N], U &&from, std::index_sequence<Is...>) {
    ((flat_array_at<Is>(to) = flat_array_at<Is>(std::forward<U>(from))), ...);
}

template <typename T, typename U, std::size_t N>
auto flat_array_assign(T (&to)[N], U(&&from)[N])
    -> std::enable_if_t<same_extents_v<T, U>> {
    flat_array_assign_impl(to, std::move(from), flat_sequence_for_array(to));
}

template <typename T, typename U, std::size_t N>
auto flat_array_assign(T (&to)[N], U const (&from)[N])
    -> std::enable_if_t<same_extents_v<T, U>> {
    flat_array_assign_impl(to, from, flat_sequence_for_array(to));
}

} // namespace detail
} // namespace tr