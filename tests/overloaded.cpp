#include <tr/overloaded.h>

#include <tr/detail/type_traits.h>
#include <tr/type_constant.h>
#include <tr/value_constant.h>

#include <functional>
#include <utility>

using tr::overloaded;
using tr::true_c;
using tr::type_c;
using tr::value_c;

namespace {

template <typename Overload>
constexpr auto is_aggregate(Overload &&) noexcept -> bool {
    using overload_t = tr::detail::remove_cvref_t<Overload>;
    return std::is_aggregate_v<overload_t>;
}

auto foo_may_throw(int) -> tr::type_constant<int> { return {}; }
auto foo_noexcept(int) noexcept -> tr::type_constant<int> { return {}; }

struct TestOverload {
    void test_lambdas() {
        // Check the call operator is constant evaluated, if the lambdas I
        // pass in can be constant evaluated
        overloaded ovrld{[](auto) { return value_c<0>; },
                         [](int) { return value_c<1>; },
                         [](char const *) { return value_c<2>; }};

        static_assert(ovrld(1.0) == 0);
        static_assert(ovrld(1) == 1);
        static_assert(ovrld("") == 2);

        static_assert(is_aggregate(ovrld));
    }

    void test_func_ptr() {
        struct Callable {
            constexpr double operator()(double) const { return 1; }
        };

        // Check I can mix lambdas, callable objects and function pointers.
        overloaded ovrld{[](auto) { return -1; }, Callable{}, foo_may_throw};
        static_assert(is_aggregate(ovrld));

        static_assert(ovrld("") == -1);
        static_assert(ovrld(42.0) == 1.0);

        // Note: ovrld(42) is not constexpr because foo is not.
        auto int_c = ovrld(42);
        static_assert(int_c == type_c<int>);
    }

    void test_std_function() {
        std::function func{foo_may_throw};
        overloaded o{func};
        auto res = o(0);
        static_assert(res == type_c<int>);
    }

    void test_mem_func_ptr() {
        struct S {
            auto foo() & -> tr::type_constant<S &> { return {}; }
            auto foo() && -> tr::type_constant<S &&> { return {}; }
            auto foo() const & -> tr::type_constant<S const &> { return {}; }
            auto foo() const && -> tr::type_constant<S const &&> { return {}; }

            auto bar() -> tr::type_constant<S> { return {}; }
            auto bar() const -> tr::type_constant<S const> { return {}; }
        };

        using foo_ptr_t = tr::type_constant<S> (S::*)();
        using cfoo_ptr_t = tr::type_constant<S const> (S::*)() const;

        using lref_foo_ptr_t = tr::type_constant<S &> (S::*)() &;
        using rref_foo_ptr_t = tr::type_constant<S &&> (S::*)() &&;
        using clref_foo_ptr_t = tr::type_constant<S const &> (S::*)() const &;
        using crref_foo_ptr_t = tr::type_constant<S const &&> (S::*)() const &&;

        S s;

        {
            overloaded o{(lref_foo_ptr_t)&S::foo, (rref_foo_ptr_t)&S::foo,
                         (clref_foo_ptr_t)&S::foo, (crref_foo_ptr_t)&S::foo};

            auto s_lref = o(s);
            static_assert(s_lref == type_c<S &>);

            auto s_clref = o(std::as_const(s));
            static_assert(s_clref == type_c<S const &>);

            auto s_rref = o(std::move(s));
            static_assert(s_rref == type_c<S &&>);

            auto s_crref = o(std::move(std::as_const(s)));
            static_assert(s_crref == type_c<S const &&>);
        }

        {
            overloaded o{(foo_ptr_t)&S::bar, (cfoo_ptr_t)&S::bar};

            auto s_non_const = o(s);
            static_assert(s_non_const == type_c<S>);

            auto s_const = o(std::as_const(s));
            static_assert(s_const == type_c<S const>);
        }
    }

    void test_mem_ptr() {
        struct S {
            int i{};
        };

        overloaded o{&S::i};

        S s;

        using int_lref_t = decltype(o(s));
        static_assert(std::is_same_v<int_lref_t, int &>);

        using int_clref_t = decltype(o(std::as_const(s)));
        static_assert(std::is_same_v<int_clref_t, int const &>);

        using int_rref_t = decltype(o(std::move(s)));
        static_assert(std::is_same_v<int_rref_t, int &&>);

        using int_crref_t = decltype(o(std::move(std::as_const(s))));
        static_assert(std::is_same_v<int_crref_t, int const &&>);
    }

    void test_compose() {
        // Check I can compose overloads
        overloaded o0{[](int) { return 0; }};
        overloaded o1{[](double) { return 1; }, [](char const *) { return 2; }};

        overloaded ovrld{o0, o1, [](auto) { return 3; }};
        static_assert(is_aggregate(ovrld));

        static_assert(ovrld(42) == 0);
        static_assert(ovrld(0.) == 1);
        static_assert(ovrld("") == 2);
        static_assert(ovrld(1u) == 3);
    }
};

} // namespace
