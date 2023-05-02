#pragma once

#include <tr/value_constant.h>

namespace tr {

/// @brief A class to treat types as if they are values.
/// @tparam T The type to treat as value-like.
template <typename T>
struct type_constant {
    using type = T;

    /// @brief Check if two types are equal.
    /// @return This overload always returns `true_c`.
    [[nodiscard]] friend constexpr auto operator==(type_constant,
                                                   type_constant) noexcept
        -> value_constant<true> {
        return {};
    }

    /// @brief Check if two types are equal.
    /// @return This overload always returns `false_c`.
    template <typename U>
    [[nodiscard]] friend constexpr auto operator==(type_constant,
                                                   type_constant<U>) noexcept
        -> value_constant<false> {
        return {};
    }

    /// @brief Check if two types differ.
    /// @tparam U The wrapped type of the left-hand side.
    /// @param lhs The left-hand side.
    /// @param rhs The right-hand side
    /// @return `true_c` if `T == U`; `false_c`, otherwise.
    template <typename U>
    [[nodiscard]] friend constexpr auto
    operator!=(type_constant lhs, type_constant<U> rhs) noexcept
        -> value_constant<!decltype(lhs == rhs)::value> {
        return {};
    }
};

/// @brief Variable template to simplify the conversion from a type `T` to a
/// `value_constant<T>`.
/// @tparam T The type to wrap.
template <typename T>
static constexpr type_constant<T> type_c{};

} // namespace tr