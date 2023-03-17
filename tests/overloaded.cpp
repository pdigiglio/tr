#include <tr/overloaded.h>
#include <tr/type_constant.h>
#include <tr/value_constant.h>

// TODO: add a `select` utility to select an overload.

using tr::overloaded;
using tr::type_c;
using tr::value_c;

namespace {

template <typename Overload>
constexpr auto is_aggregate(Overload) noexcept {
    return value_c<std::is_aggregate_v<Overload>>;
}

constexpr static auto true_c = value_c<true>;
constexpr static auto false_c = value_c<false>;

int foo(int) { return {}; }

struct TestOverload {
    void test_lambdas() {
        // Check the call operator is constant evaluated, if the lambdas I
        // pass in can be constant evaluated
        overloaded ovrld{[](auto) { return 0; }, [](int) { return 1; },
                       [](char const *) { return 2; }};

        static_assert(ovrld(1.0) == 0);
        static_assert(ovrld(1) == 1);
        static_assert(ovrld("") == 2);

        auto const isAgg = is_aggregate(ovrld);
        static_assert(isAgg == true_c);
    }

    void test_func_ptr() {
        struct Callable {
            constexpr double operator()(double) const { return 1; }
        };

        // Check I can mix lambdas, callable objects and function pointers.
        overloaded ovrld{[](auto) { return -1; }, Callable{}, foo};

        static_assert(ovrld("") == -1);
        static_assert(ovrld(42.0) == 1.0);

        // foo is not constexpr
        //static_assert(ovrld(42) == 0);
        static_assert(type_c<decltype(ovrld(42))> == type_c<int>);

        auto const isAgg = is_aggregate(ovrld);
        static_assert(isAgg == true_c);
    }

    void test_compose() {
        // Check I can compose overloads
        overloaded o0{[](int) { return 0; }};
        overloaded o1{[](double) { return 1; }, [](char const *) { return 2; }};

        overloaded ovrld{o0, o1, [](auto) { return 3; }};
        static_assert(ovrld(42) == 0);
        static_assert(ovrld(0.) == 1);
        static_assert(ovrld("") == 2);
        static_assert(ovrld(1u) == 3);

        auto const isAgg = is_aggregate(ovrld);
        static_assert(isAgg == true_c);
    }
};

} // namespace