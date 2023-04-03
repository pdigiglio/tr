#include <tr/value_constant.h>

#include <climits>
#include <tr/type_constant.h>

using tr::false_c;
using tr::true_c;
using tr::type_c;
using tr::value_c;

namespace {

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
        static_assert(type_c<decltype(a - b)::type> == type_c<unsigned>);
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
};

} // namespace