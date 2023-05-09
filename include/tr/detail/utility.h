#pragma once

#include <tr/detail/literal_parser.h>

#include <type_traits>
#include <utility>

namespace tr {
namespace detail {
template <class T, class U>
[[nodiscard]] constexpr auto &&forward_like(U &&x) noexcept {
    constexpr bool is_adding_const =
        std::is_const_v<std::remove_reference_t<T>>;
    if constexpr (std::is_lvalue_reference_v<T &&>) {
        if constexpr (is_adding_const)
            return std::as_const(x);
        else
            return static_cast<U &>(x);
    } else {
        if constexpr (is_adding_const)
            return std::move(std::as_const(x));
        else
            return std::move(x);
    }
}

/// @brief Forward `t` as a l-value reference, if it's a l-value reference;
/// or return a move-constructed instance of `T`, if `t` is a r-value
/// reference.
///
/// @detail This function is useful to avoid dangling r-value references.
///
/// @tparam T
/// @param t The value to forward or materialize.
template <typename T>
static constexpr auto lref_or_value(T &&t) -> T {
    return static_cast<T &&>(t);
}

} // namespace detail

template <int I>
constexpr std::integral_constant<int, I> ic{};

template <std::size_t I>
constexpr std::integral_constant<std::size_t, I> zuic{};

namespace literals {

/// @brief User-defined literal operator for `tr::integral_constant<std::size_t,
/// _>`, where `_` is the parsed literal value.
template <char... Chars>
[[nodiscard]] constexpr auto operator""_zuic() noexcept
    -> detail::integral_parser_t<std::size_t, Chars...> {
    return {};
}

/// @brief User-defined literal operator for `tr::integral_constant<unsigned,
/// _>`, where `_` is the parsed literal value.
template <char... Chars>
[[nodiscard]] constexpr auto operator""_uic() noexcept
    -> detail::integral_parser_t<unsigned, Chars...> {
    return {};
}

/// @brief User-defined literal operator for `tr::integral_constant<int, _>`,
/// where `_` is the parsed literal value.
template <char... Chars>
[[nodiscard]] constexpr auto operator""_ic() noexcept
    -> detail::integral_parser_t<int, Chars...> {
    return {};
}

/// @brief User-defined literal operator for `tr::integral_constant<int, _>`,
/// where `_` is the parsed literal value.
template <char... Chars>
[[nodiscard]] constexpr auto operator""_c() noexcept
    -> detail::integral_parser_t<int, Chars...> {
    return {};
}

} // namespace literals

} // namespace tr