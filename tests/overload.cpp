#include <tr/overload.h>
#include <tr/type_constant.h>

using tr::overload;
using tr::type_c;

namespace {

void foo(int) {}

struct TestOverload {
    void test_lambdas() {
        // Check the call operator is constant evaluated, if the lambdas I
        // pass in can be constant evaluated
        overload o{[](auto) { return 0; }, [](int) { return 1; },
                   [](char const *) { return 2; }};

        static_assert(o(1.0) == 0);
        static_assert(o(1) == 1);
        static_assert(o("") == 2);
    }

    void test_func_ptr() {
        struct Callable {
            constexpr double operator()(double) const { return {}; }
        };

        // Check I can mix lambdas, callable objects and function pointers.
        overload o{[](auto) { return 0; }, Callable{}, foo};

        static_assert(type_c<decltype(o(42))> == type_c<void>);

        static_assert(type_c<decltype(o(""))> == type_c<int>);
        static_assert(o("") == 0);

        static_assert(o(42.0) == 0.0);
    }

    void test_compose() {
        // Check I can compose overloads
        overload o0{[](int) { return 0; }};
        overload o1{[](double) { return 1; }, [](char const *) { return 2; }};

        overload o{o0, o1, [](auto) { return 3; }};
        static_assert(o(42) == 0);
        static_assert(o(0.) == 1);
        static_assert(o("") == 2);
        static_assert(o(1u) == 3);
    }
};

} // namespace