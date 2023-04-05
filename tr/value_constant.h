#pragma once

#include <cstddef>
#include <type_traits>

namespace tr {

namespace detail {

template <template <auto...> typename, typename, auto...>
struct is_valid_value_expr : std::false_type {};

template <template <auto...> typename ValueExpr, auto... Vals>
struct is_valid_value_expr<ValueExpr, std::void_t<ValueExpr<Vals...>>, Vals...>
    : std::true_type {};

} // namespace detail

template <template <auto...> typename ValueExpr, auto... Vals>
static constexpr bool is_valid_value_expr_v{
    detail::is_valid_value_expr<ValueExpr, void, Vals...>::value};


template <auto Val>
struct value_constant {
    using type = decltype(Val);
    static constexpr type value{Val};

    /// @brief Implicit cast to the wrapped value.
    /// @return The wrapped value.
    [[nodiscard]] constexpr operator type() const noexcept { return Val; }

    /// @brief Explicit cast to the wrapped value.
    /// @tparam T The type to cast to (it's different from the wrapped value type).
    /// @return The wrapped value.
    template <typename T, typename = std::enable_if_t<!std::is_same_v<T, type>>>
    [[nodiscard]] constexpr explicit operator T() const noexcept {
        return T{Val};
        //     ^ prevent narrowing conversions (on explicit casts).
    }

#define DEFINE_INFIX_BINARY_OPERATOR(OP, EXPR_TYPE_NAME)                       \
  private:                                                                     \
    template <auto Val_>                                                       \
    using EXPR_TYPE_NAME = value_constant<(Val OP Val_)>;                      \
                                                                               \
  public:                                                                      \
    template <auto Val_>                                                       \
    [[nodiscard]] friend constexpr auto operator OP(                           \
        value_constant, value_constant<Val_>) noexcept->EXPR_TYPE_NAME<Val_> { \
        return {};                                                             \
    }                                                                          \
                                                                               \
    template <auto Val_>                                                       \
    friend auto operator OP(value_constant, value_constant<Val_>)              \
        ->std::enable_if_t<!is_valid_value_expr_v<EXPR_TYPE_NAME, Val_>> =     \
        delete; \

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
    template <auto Val>                                                        \
    [[nodiscard]] constexpr auto operator OP(                                  \
        value_constant<Val>) noexcept->detail::EXPR_TYPE_NAME<Val> {           \
        return {};                                                             \
    }                                                                          \
                                                                               \
    template <auto Val>                                                        \
    auto operator OP(value_constant<Val>)                                      \
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
