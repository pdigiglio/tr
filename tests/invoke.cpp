#include <tr/invoke.h>

#include <tr/type_constant.h>

#include <functional>
#include <type_traits>

namespace {

struct NoCopyNorMove {
    NoCopyNorMove() = default;
    NoCopyNorMove(NoCopyNorMove &&) = delete;
    NoCopyNorMove(NoCopyNorMove const &) = delete;

    static void by_value(NoCopyNorMove) {}
};

struct Func {
    Func(Func &&) = delete;
    Func(Func const &) = delete;

    constexpr auto operator()(int i) & {
        if (i)
            throw 0;
        return tr::type_c<Func &>;
    }
    constexpr auto operator()() &&noexcept { return tr::type_c<Func &&>; }
    constexpr auto operator()() const & { return tr::type_c<Func const &>; }
    constexpr auto operator()() const && { return tr::type_c<Func const &&>; }

    auto f_mut_lref_may_throw() & { return tr::type_c<Func &>; }
    auto f_mut_lref_noexcept() &noexcept { return tr::type_c<Func &>; }

    auto f_mut_rref_may_throw() && { return tr::type_c<Func &&>; }
    auto f_mut_rref_noexcept() &&noexcept { return tr::type_c<Func &&>; }

    auto f_mut_may_throw() { return tr::type_c<Func>; }
    auto f_mut_noexcept() noexcept { return tr::type_c<Func>; }

    auto f_const_lref_may_throw() const & { return tr::type_c<Func const &>; }
    auto f_const_lref_noexcept() const &noexcept {
        return tr::type_c<Func const &>;
    }

    auto f_const_rref_may_throw() const && { return tr::type_c<Func const &&>; }
    auto f_const_rref_noexcept() const &&noexcept {
        return tr::type_c<Func const &&>;
    }

    auto f_const_may_throw() const { return tr::type_c<Func const>; }
    auto f_const_noexcept() const noexcept { return tr::type_c<Func const>; }

    // void foo() noexcept {}

    int Data_;
};

auto foo_may_throw(int) -> tr::type_constant<int> { return {}; }
auto foo_noexcept(int) noexcept -> tr::type_constant<int> { return {}; }

struct TestInvoke {

    void test_invoke() {

        Func f{};

        {
            constexpr auto res = tr::invoke(std::move(f));
            static_assert(res == tr::type_c<Func &&>);

            static_assert(noexcept(tr::invoke(std::move(f))));
            static_assert(noexcept(std::invoke(std::move(f))));
        }

        {
            auto res = tr::invoke(f, 0);
            static_assert(res == tr::type_c<Func &>);
            static_assert(!std::is_nothrow_invocable_v<Func &, int>);

#if defined(_MSC_VER)
            // NOTE: MSVC seems to be too clever about what is noexcept and
            // what isn't, if `f` is `constexpr`. However, it's non conforming:
            static_assert(noexcept(tr::invoke(f, 0)));
            static_assert(noexcept(std::invoke(f, 0)));
#else
            static_assert(!noexcept(tr::invoke(f, 0)));
            static_assert(!noexcept(std::invoke(f, 0)));
#endif

            static_assert(!noexcept(tr::invoke(f, 1)));
            static_assert(!noexcept(std::invoke(f, 1)));

            int i{};
            static_assert(!noexcept(tr::invoke(f, i)));
            static_assert(!noexcept(std::invoke(f, i)));
        }

        {
            auto res = tr::invoke(std::move(std::as_const(f)));
            static_assert(res == tr::type_c<Func const &&>);
        }

        {
            auto res = tr::invoke(std::as_const(f));
            static_assert(res == tr::type_c<Func const &>);
        }

        {
            auto res = tr::invoke(foo_may_throw, 1);
            static_assert(res == tr::type_c<int>);
            static_assert(!noexcept(tr::invoke(foo_may_throw, 1)));
        }

        {
            auto res = tr::invoke(foo_noexcept, 1);
            static_assert(res == tr::type_c<int>);
            static_assert(noexcept(tr::invoke(foo_noexcept, 1)));
        }

        {
            NoCopyNorMove::by_value(NoCopyNorMove{}); // Ok

            // Turns out you can't std::invoke NoCopyNorMove::by_value.
            // These calls don't compile:
            //
            // std::invoke(NoCopyNorMove::by_value, NoCopyNorMove{});
            // invoke(NoCopyNorMove::by_value, NoCopyNorMove{});
        }

        {
            auto callable = &Func::f_mut_lref_may_throw;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func &>);
            static_assert(!noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_mut_lref_noexcept;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func &>);
            static_assert(noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_mut_rref_may_throw;
            auto res = tr::invoke(callable, std::move(f));
            static_assert(res == tr::type_c<Func &&>);
            static_assert(!noexcept(tr::invoke(callable, std::move(f))));
        }

        {
            auto callable = &Func::f_mut_rref_noexcept;
            auto res = tr::invoke(callable, std::move(f));
            static_assert(res == tr::type_c<Func &&>);
            static_assert(noexcept(tr::invoke(callable, std::move(f))));
        }

        {
            auto callable = &Func::f_mut_may_throw;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func>);
            static_assert(!noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_mut_noexcept;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func>);
            static_assert(noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_const_lref_may_throw;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func const &>);
            static_assert(!noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_const_lref_noexcept;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func const &>);
            static_assert(noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_const_rref_may_throw;
            auto res = tr::invoke(callable, std::move(f));
            static_assert(res == tr::type_c<Func const &&>);
            static_assert(!noexcept(tr::invoke(callable, std::move(f))));
        }

        {
            auto callable = &Func::f_const_rref_noexcept;
            auto res = tr::invoke(callable, std::move(f));
            static_assert(res == tr::type_c<Func const &&>);
            static_assert(noexcept(tr::invoke(callable, std::move(f))));
        }

        {
            auto callable = &Func::f_const_may_throw;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func const>);
            static_assert(!noexcept(tr::invoke(callable, f)));
        }

        {
            auto callable = &Func::f_const_noexcept;
            auto res = tr::invoke(callable, f);
            static_assert(res == tr::type_c<Func const>);
            static_assert(noexcept(tr::invoke(callable, f)));
        }

        {
            auto same_types = [](auto &&lhs, auto &&rhs) {
                using same_types_t = std::is_same<decltype(lhs), decltype(rhs)>;
                return typename same_types_t::type{};
            };

            auto callable = &Func::Data_;

            auto sameTypes = same_types(tr::invoke(callable, f), f.Data_);
            static_assert(sameTypes);

            // sameTypes is std::true_type. So I don't need to static_assert:
            // if I can assign to sameTypes, then same_types() returned
            // std::true_type.
            sameTypes = same_types(tr::invoke(callable, std::as_const(f)),
                                   std::as_const(f).Data_);
            sameTypes = same_types(tr::invoke(callable, std::move(f)),
                                   std::move(f).Data_);
            sameTypes =
                same_types(tr::invoke(callable, std::move(std::as_const(f))),
                           std::move(std::as_const(f)).Data_);
        }
    }
};
} // namespace