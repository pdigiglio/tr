#pragma once

#include <cstddef>
#include <type_traits>

namespace tr {

template <auto Val>
struct value_constant {
    using type = decltype(Val);
    static constexpr type value{Val};

    [[nodiscard]] constexpr operator type() const noexcept { return Val; }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<T, type>>>
    [[nodiscard]] constexpr explicit operator T() const noexcept {
        return T{Val};
        //     ^ prevent narrowing conversions (on explicit casts).
    }

    // TODO: make the operators SFINAE friendly.

#define DEFINE_INFIX_BINARY_OPERATOR(OP)                                       \
    template <auto Val_>                                                       \
    [[nodiscard]] friend constexpr auto operator OP(                           \
        value_constant, value_constant<Val_>) noexcept {                       \
        return value_constant<(Val OP Val_)>{};                                \
    }

    // Comparators
    DEFINE_INFIX_BINARY_OPERATOR(==)
    DEFINE_INFIX_BINARY_OPERATOR(!=)
    DEFINE_INFIX_BINARY_OPERATOR(<)
    DEFINE_INFIX_BINARY_OPERATOR(<=)
    DEFINE_INFIX_BINARY_OPERATOR(>)
    DEFINE_INFIX_BINARY_OPERATOR(>=)

    // Arithmetic
    DEFINE_INFIX_BINARY_OPERATOR(+)
    DEFINE_INFIX_BINARY_OPERATOR(-)
    DEFINE_INFIX_BINARY_OPERATOR(*)
    DEFINE_INFIX_BINARY_OPERATOR(/)
    DEFINE_INFIX_BINARY_OPERATOR(%)

    // Logical
    DEFINE_INFIX_BINARY_OPERATOR(&&)
    DEFINE_INFIX_BINARY_OPERATOR(||)

    // Question to self: Do logical operators need to short-circuit?
    //
    // Likely, no: value_constant<bool> results from a compile-time computation
    // that has no side effects and has no runtime cost.

    // Bitwise
    DEFINE_INFIX_BINARY_OPERATOR(&)
    DEFINE_INFIX_BINARY_OPERATOR(|)
    DEFINE_INFIX_BINARY_OPERATOR(^)
    DEFINE_INFIX_BINARY_OPERATOR(<<)
    DEFINE_INFIX_BINARY_OPERATOR(>>)

#undef DEFINE_INFIX_BINARY_OPERATOR
};

#define DEFINE_PREFIX_UNARY_OPERATOR(OP)                                       \
    template <auto Val>                                                        \
    [[nodiscard]] constexpr auto operator OP(value_constant<Val>) noexcept {   \
        return value_constant<(OP Val)>{};                                     \
    }

DEFINE_PREFIX_UNARY_OPERATOR(!)
DEFINE_PREFIX_UNARY_OPERATOR(~)
DEFINE_PREFIX_UNARY_OPERATOR(+)
DEFINE_PREFIX_UNARY_OPERATOR(-)

#undef DEFINE_PREFIX_UNARY_OPERATOR

template <auto Val>
static constexpr value_constant<Val> value_c{};

static constexpr auto true_c = value_c<true>;
static constexpr auto false_c = value_c<false>;

template <unsigned Val>
static constexpr value_constant<Val> uic{};

} // namespace tr
