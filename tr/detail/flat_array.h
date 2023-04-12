#pragma once

#include <cstdint>
#include <utility>

namespace tr {
namespace detail {

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

} // namespace detail
} // namespace tr