#pragma once

#include "detail/type_traits.h"
#include "detail/utility.h"
#include "value_constant.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tr {
namespace detail {

/// @brief A utility class that's implicitly constructible from any other type.
/// The common type between `from_any` and any other type is `from_any`.
struct from_any {

    /// @brief Implicit (undefined) constructor from any type.
    /// @tparam T 
    /// @param  
    template <typename T>
    /* implicit */ constexpr from_any(T&&) noexcept /* undefined */;
};

template <std::size_t I, auto Val>
struct value_sequence_elem {
    template <typename T>
    [[nodiscard]] constexpr auto
    operator[](std::integral_constant<T, I>) const noexcept
        -> value_constant<Val> {
        return {};
    }
};

template <typename T, typename IdxPack, auto... Vals>
struct value_sequence_impl;

template <typename T, std::size_t... Is, auto... Vals>
struct value_sequence_impl<T, std::index_sequence<Is...>, Vals...>
    : value_sequence_elem<Is, Vals>... {
    static_assert(
        are_same_v<T, detail::from_any> || are_same_v<T, decltype(Vals)...>,
        "This instanciation is neither a value tuple nor a value array.");

    using value_sequence_elem<Is, Vals>::operator[]...;
};
} // namespace detail

template <typename T, auto... Vals>
struct value_sequence
    : detail::value_sequence_impl<T, std::make_index_sequence<sizeof...(Vals)>,
                                  Vals...> {};

//template <typename T, auto... Vals>
//struct tup_size<value_sequence<T, Vals...>>
//    : std::integral_constant<std::size_t, sizeof...(Vals)> {};

/// @brief Get the `I`-th element of the `value_sequence`
/// @return
template <std::size_t I, typename T, std::size_t... Is, auto... Vals>
[[nodiscard]] constexpr auto get(value_sequence<T, Vals...> vs) noexcept {
    static_assert(I < sizeof...(Vals));
    std::integral_constant<std::size_t, I> idx{};
    return vs[idx];
}

template <typename T, auto... Vals0, typename U, auto... Vals1>
constexpr auto operator+(value_sequence<T, Vals0...>,
                         value_sequence<U, Vals1...>) noexcept
    -> value_sequence<std::common_type_t<T, U>, Vals0..., Vals1...> {
    return {};
}

template <typename T, auto... Vals0, auto Val>
constexpr auto operator+(value_sequence<T, Vals0...>,
                         value_constant<Val>) noexcept
    -> value_sequence<std::common_type_t<T, decltype(Val)>, Vals0 + Val...> {
    return {};
}

template <typename T, auto... Vals0, auto Val>
constexpr auto operator+(value_constant<Val> val,
                         value_sequence<T, Vals0...> tup) noexcept
    -> decltype(tup + val) {
    return {};
}

/// @brief A sequence of homogeneous values.
template <typename T, T... Vals>
using value_array_constant = value_sequence<T, Vals...>;

/// @brief An instance of a sequence of homogeneous value.
/// @tparam T The value type.
template <typename T, T... Vals>
static constexpr value_array_constant<T, Vals...> array_c{};

/// @brief A sequence of (possibly) heterogeneous values.
template <auto... Vals>
using value_tuple_constant = value_sequence<detail::from_any, Vals...>;

/// @brief An instance of a sequence of (possibly) heterogeneous values.
template <auto... Vals>
static constexpr value_tuple_constant<Vals...> tuple_c{};
} // namespace tr

namespace std {

template <typename T, auto... Vals>
struct tuple_size<::tr::value_sequence<T, Vals...>>
    : std::integral_constant<std::size_t, sizeof...(Vals)> {};

template <std::size_t I, typename T, auto... Vals>
struct tuple_element<I, ::tr::value_sequence<T, Vals...>> {
    using type = decltype(::tr::value_sequence<T, Vals...>{}[tr::ic<I>]);
};
} // namespace std
