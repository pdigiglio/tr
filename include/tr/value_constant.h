#pragma once

#include <tr/fwd/value_constant.h>
#include <tr/is_valid.h>

#include <cstddef>
#include <type_traits>

namespace tr {

template <typename T, T Val>
struct integral_constant : std::integral_constant<T, Val> {

    /// @brief Explicit cast to the wrapped value.
    /// @tparam U The type to cast to (different from the wrapped value type).
    /// @return The wrapped value.
    template <typename U, typename = std::enable_if_t<!std::is_same_v<U, T>>>
    [[nodiscard]] constexpr explicit operator U() const noexcept {
        return U{Val};
        //     ^ prevent narrowing conversions (on explicit casts).
    }

#define DEFINE_INFIX_BINARY_OPERATOR(OP, EXPR_TYPE_NAME)                       \
  private:                                                                     \
    template <auto Val_>                                                       \
    using EXPR_TYPE_NAME = value_constant<(Val OP Val_)>;                      \
                                                                               \
  public:                                                                      \
    template <typename T_, T_ Val_>                                            \
    [[nodiscard]] friend constexpr auto operator OP(                           \
        integral_constant,                                                     \
        integral_constant<T_, Val_>) noexcept->EXPR_TYPE_NAME<Val_> {          \
        return {};                                                             \
    }                                                                          \
                                                                               \
    template <typename T_, T_ Val_>                                            \
    friend auto operator OP(integral_constant, integral_constant<T_, Val_>)    \
        ->std::enable_if_t<!is_valid_value_expr_v<EXPR_TYPE_NAME, Val_>> =     \
        delete;

    // Comparators
    DEFINE_INFIX_BINARY_OPERATOR(==, equal_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(!=, not_equal_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(<, less_than_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(<=, less_or_equal_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(>, greater_than_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(>=, greater_or_equal_expr_t)

    // Arithmetic
    DEFINE_INFIX_BINARY_OPERATOR(+, plus_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(-, minus_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(*, multiplication_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(/, division_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(%, modulo_expr_t)

    // Logical
    DEFINE_INFIX_BINARY_OPERATOR(&&, logic_and_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(||, logic_or_expr_t)

    // Question to self: Do logical operators need to short-circuit?
    //
    // Likely, no: value_constant<bool> results from a compile-time computation
    // that has no side effects and has no runtime cost.

    // Bitwise
    DEFINE_INFIX_BINARY_OPERATOR(&, bitwise_and_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(|, bitwise_or_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(^, bitwise_xor_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(<<, left_shift_expr_t)
    DEFINE_INFIX_BINARY_OPERATOR(>>, right_shift_expr_t)

#undef DEFINE_INFIX_BINARY_OPERATOR
};

#define DEFINE_PREFIX_UNARY_OPERATOR(OP, EXPR_TYPE_NAME)                       \
    namespace detail {                                                         \
    template <auto Val>                                                        \
    using EXPR_TYPE_NAME = value_constant<(OP Val)>;                           \
    }                                                                          \
                                                                               \
    template <typename T, T Val>                                               \
    [[nodiscard]] constexpr auto operator OP(                                  \
        integral_constant<T, Val>) noexcept->detail::EXPR_TYPE_NAME<Val> {     \
        return {};                                                             \
    }                                                                          \
                                                                               \
    template <typename T, T Val>                                               \
    auto operator OP(integral_constant<T, Val>)                                \
        ->std::enable_if_t<                                                    \
            !is_valid_value_expr_v<detail::EXPR_TYPE_NAME, Val>> = delete;

DEFINE_PREFIX_UNARY_OPERATOR(!, value_constant_logical_not_expr_t)
DEFINE_PREFIX_UNARY_OPERATOR(~, value_constant_bitwise_not_expr_t)
DEFINE_PREFIX_UNARY_OPERATOR(+, value_constant_unary_plus_expr_t)
DEFINE_PREFIX_UNARY_OPERATOR(-, value_constant_unary_minus_expr_t)

#undef DEFINE_PREFIX_UNARY_OPERATOR

/// @brief A variable template to simplify the creation of
/// `value_constant<Val>` instances.
/// @tparam Val The wrapped value.
template <auto Val>
static constexpr value_constant<Val> value_c{};

/// @brief A variable template to wrap `true`.
static constexpr auto true_c = value_c<true>;

/// @brief A variable template to wrap `false`.
static constexpr auto false_c = value_c<false>;

template <unsigned Val>
static constexpr value_constant<Val> uic{};

} // namespace tr
