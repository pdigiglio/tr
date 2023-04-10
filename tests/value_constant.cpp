#include <tr/value_constant.h>

#include <tr/detail/type_traits.h>
#include <tr/type_constant.h>

#include <climits>
#include <cstdint>

using tr::false_c;
using tr::true_c;
using tr::type_c;
using tr::value_c;
using tr::detail::check_expr;

#define CHECK_BINARY_EXPR(OP)                                                  \
    check_expr([](auto lhs, auto rhs) -> decltype(lhs OP rhs) {})

#define CHECK_UNARY_EXPR(OP) check_expr([](auto arg) -> decltype(OP arg) {})

namespace {

enum struct dummy_enum { zero };

enum struct bitmask_enum : std::uint8_t {
    zero = 0,
    one = (1 << 0),
    two = (1 << 1),
    three = one | two,
    four = (1 << 2),
    max = static_cast<std::uint8_t>(-1)
};

#define BITMASK_BITWISE_OP(ENUM, OP)                                           \
    constexpr ENUM operator OP(ENUM lhs, ENUM rhs) {                           \
        using underlying_t = std::underlying_type_t<ENUM>;                     \
        return static_cast<ENUM>(static_cast<underlying_t>(lhs)                \
                                     OP static_cast<underlying_t>(rhs));       \
    }

BITMASK_BITWISE_OP(bitmask_enum, &)
BITMASK_BITWISE_OP(bitmask_enum, |)
BITMASK_BITWISE_OP(bitmask_enum, ^)

#undef BITMASK_BITWISE_OP

#define BITMASK_SHIFT_OP(ENUM, OP)                                             \
    template <typename Int>                                                    \
    constexpr auto operator OP(ENUM e, Int shift)                              \
        ->std::enable_if_t<std::is_integral_v<Int>, ENUM> {                    \
        using underlying_t = std::underlying_type_t<ENUM>;                     \
        return static_cast<ENUM>(static_cast<underlying_t>(e) OP shift);       \
    }

BITMASK_SHIFT_OP(bitmask_enum, <<)
BITMASK_SHIFT_OP(bitmask_enum, >>)

#undef BITMASK_SHIFT_OP

constexpr bitmask_enum operator~(bitmask_enum e) noexcept {
    using underlying_t = std::underlying_type_t<bitmask_enum>;
    return static_cast<bitmask_enum>(~static_cast<underlying_t>(e));
}

template <typename UInt0, typename Uint1>
constexpr UInt0 pow(UInt0 base, Uint1 exp) noexcept {
    UInt0 res{1};
    for (Uint1 e{}; e != exp; ++e)
        res *= base;
    return res;
}

struct TestValueConstant {
    void test() {
        auto a = value_c<4u>;
        auto b = value_c<5>;

        // Comparators
        static_assert(a == a);
        static_assert(a != b);
        static_assert(a < b);
        static_assert(a <= a);
        static_assert(b > a);
        static_assert(a >= a);

        // Arithmetic
        static_assert(a + b == 9);
        static_assert(a - b == UINT_MAX);
        static_assert(type_c<decltype(a - b)::value_type> == type_c<unsigned>);
        static_assert(a * b == 20);
        static_assert(a / b == 0);
        static_assert(a % b == a);

        // Logical
        auto t = true_c;
        auto f = false_c;
        static_assert(t && t);
        static_assert(!(f && t));
        static_assert(!(t && f));
        static_assert(!(f && f));

        static_assert(t || t);
        static_assert(f || t);
        static_assert(t || f);
        static_assert(!(f || f));

        // Bitwise
        static_assert((a & b) == a);
        static_assert((a | b) == b);
        static_assert((a ^ b) == 1);
        static_assert((a << b) == a * pow(2, b.value));
        static_assert((b >> a) == 0);

        // UNARY
        static_assert(-a == (UINT_MAX - a + 1));
        static_assert(-b == -5);
        static_assert(~a == -(a + 1));
        static_assert(+a == a);
    }

    void test_sfinae() {

        {
            auto zero = value_c<bitmask_enum::zero>;
            auto one = value_c<bitmask_enum::one>;
            auto two = value_c<bitmask_enum::two>;
            auto three = value_c<bitmask_enum::three>;
            auto four = value_c<bitmask_enum::four>;
            auto max = value_c<bitmask_enum::max>;

            static_assert((one & two) == zero);
            static_assert((one ^ three) == two);
            static_assert((one | two) == three);
            static_assert(~zero == max);

            static_assert((two >> value_c<1>) == one);
            static_assert((two << value_c<1>) == four);

// MSVC allows all of the meaningless constexpr calculations below.
#ifndef _MSC_VER
            static_assert(!CHECK_BINARY_EXPR(<<)(one, value_c<-1>));
            static_assert(!CHECK_BINARY_EXPR(>>)(one, value_c<-1>));
#endif // _MSC_VER
        }

        {
// MSVC allows all of the meaningless constexpr calculations below.
#ifndef _MSC_VER
            static_assert(!CHECK_BINARY_EXPR(/)(value_c<1>, value_c<0>));
            static_assert(CHECK_BINARY_EXPR(/)(value_c<1>, value_c<1>));

            static_assert(!CHECK_BINARY_EXPR(%)(value_c<1>, value_c<0>));
            static_assert(CHECK_BINARY_EXPR(%)(value_c<1>, value_c<1>));

            // A dummy enum with that doesn't support operators.

            auto a = value_c<dummy_enum{}>;
            auto b = value_c<1>;
            static_assert(!CHECK_BINARY_EXPR(==)(a, b));
            static_assert(!CHECK_BINARY_EXPR(!=)(a, b));
            static_assert(!CHECK_BINARY_EXPR(<)(a, b));
            static_assert(!CHECK_BINARY_EXPR(<=)(a, b));
            static_assert(!CHECK_BINARY_EXPR(>)(a, b));
            static_assert(!CHECK_BINARY_EXPR(>=)(a, b));

            static_assert(!CHECK_BINARY_EXPR(+)(a, b));
            static_assert(!CHECK_BINARY_EXPR(-)(a, b));
            static_assert(!CHECK_BINARY_EXPR(*)(a, b));
            static_assert(!CHECK_BINARY_EXPR(/)(a, b));
            static_assert(!CHECK_BINARY_EXPR(%)(a, b));

            // Logical
            static_assert(!CHECK_BINARY_EXPR(&&)(a, b));
            static_assert(!CHECK_BINARY_EXPR(||)(a, b));

            // Bitwise
            static_assert(!CHECK_BINARY_EXPR(&)(a, b));
            static_assert(!CHECK_BINARY_EXPR(|)(a, b));
            static_assert(!CHECK_BINARY_EXPR(^)(a, b));
            static_assert(!CHECK_BINARY_EXPR(<<)(a, b));
            static_assert(!CHECK_BINARY_EXPR(>>)(a, b));

            // UNARY
            static_assert(!CHECK_UNARY_EXPR(-)(a));
            static_assert(!CHECK_UNARY_EXPR(~)(a));
            static_assert(!CHECK_UNARY_EXPR(+)(a));
#endif // _MSC_VER
        }
    }
};

} // namespace

#undef CHECK_BINARY_EXPR
#undef CHECK_UNARY_EXPR
