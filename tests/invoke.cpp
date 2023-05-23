#include <tr/invoke.h>

#include <tr/type_constant.h>
#include <tr/type_pack.h>

#include <functional>
#include <type_traits>

namespace {

struct FwdToTypePack {
    template <typename... Args>
    auto operator()(Args &&...) const noexcept -> tr::type_pack<Args &&...> {
        return {};
    }
};

struct NoCopyNorMove {
    NoCopyNorMove() = default;
    NoCopyNorMove(NoCopyNorMove &&) = delete;
    NoCopyNorMove(NoCopyNorMove const &) = delete;

    static void by_value(NoCopyNorMove) {}
};

struct Func {
    Func(Func &&) = delete;
    Func(Func const &) = delete;

    constexpr auto operator()(int i = 0) & {
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

template <typename ExpectedResT, bool ExpectedNoExcept, typename Callable,
          typename Obj>
void test_member_ptr(Callable callable, Obj &&obj) {

    {
        using tr::invoke;

        auto res = invoke(callable, static_cast<Obj &&>(obj));
        static_assert(std::is_same_v<decltype(res), ExpectedResT>,
                      "result type is not the expected one");

        constexpr bool nx{noexcept(invoke(callable, static_cast<Obj &&>(obj)))};
        static_assert(nx == ExpectedNoExcept, "noexcept test failed");
    }

    {
        using std::invoke;

        auto res = invoke(callable, static_cast<Obj &&>(obj));
        static_assert(std::is_same_v<decltype(res), ExpectedResT>,
                      "result type is not the expected one");

        constexpr bool nx{noexcept(invoke(callable, static_cast<Obj &&>(obj)))};
        static_assert(nx == ExpectedNoExcept, "noexcept test failed");
    }
}

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
            using res_t = tr::type_constant<Func &>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_mut_lref_may_throw, f);
        }

        {
            using res_t = tr::type_constant<Func &>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_mut_lref_may_throw,
                                       std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func &>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_mut_lref_noexcept, f);
        }

        {
            using res_t = tr::type_constant<Func &>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_mut_lref_noexcept, std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func &&>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_mut_rref_may_throw,
                                       std::move(f));
        }

        {
            using res_t = tr::type_constant<Func &&>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_mut_rref_noexcept,
                                       std::move(f));
        }

        {
            using res_t = tr::type_constant<Func>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_mut_may_throw, f);
        }

        {
            using res_t = tr::type_constant<Func>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_mut_may_throw, std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_mut_noexcept, f);
        }

        {
            using res_t = tr::type_constant<Func>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_mut_noexcept, std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func const &>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_const_lref_may_throw, f);
        }

        {
            using res_t = tr::type_constant<Func const &>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_const_lref_may_throw,
                                       std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func const &>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_const_lref_noexcept, f);
        }

        {
            using res_t = tr::type_constant<Func const &>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_const_lref_noexcept,
                                       std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func const &&>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_const_rref_may_throw,
                                       std::move(f));
        }

        {
            using res_t = tr::type_constant<Func const &&>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_const_rref_noexcept,
                                       std::move(f));
        }

        {
            using res_t = tr::type_constant<Func const>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_const_may_throw, f);
        }

        {
            using res_t = tr::type_constant<Func const>;
            constexpr bool nx{false};
            test_member_ptr<res_t, nx>(&Func::f_const_may_throw, std::ref(f));
        }

        {
            using res_t = tr::type_constant<Func const>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_const_noexcept, f);
        }

        {
            using res_t = tr::type_constant<Func const>;
            constexpr bool nx{true};
            test_member_ptr<res_t, nx>(&Func::f_const_noexcept, std::ref(f));
        }

        {
            auto same_types = [](auto &&lhs, auto &&rhs) {
                using same_types_t = std::is_same<decltype(lhs), decltype(rhs)>;
                return typename same_types_t::type{};
            };

            auto memPtr = &Func::Data_;

            auto sameTypes = same_types(tr::invoke(memPtr, f), f.Data_);
            static_assert(sameTypes);

            // sameTypes is std::true_type. So I don't need to static_assert:
            // if I can assign to sameTypes, then same_types() returned
            // std::true_type.
            sameTypes = same_types(tr::invoke(memPtr, std::as_const(f)),
                                   std::as_const(f).Data_);
            sameTypes = same_types(tr::invoke(memPtr, std::move(f)),
                                   std::move(f).Data_);
            sameTypes =
                same_types(tr::invoke(memPtr, std::move(std::as_const(f))),
                           std::move(std::as_const(f)).Data_);

            sameTypes = same_types(tr::invoke(memPtr, std::ref(f)), f.Data_);
            sameTypes = same_types(tr::invoke(memPtr, std::cref(f)),
                                   std::as_const(f).Data_);
        }

        {
            int i{};
            auto std_res = std::invoke(FwdToTypePack{}, std::ref(i));
            auto tr_res = tr::invoke(FwdToTypePack{}, std::ref(i));

            // Ok: invoke only decays std::reference_wrapper<T> to T& when
            // calling pointer to member functions/variables on T&
            static_assert(std_res == tr_res);
            static_assert(std_res ==
                          tr::type_pack_c<std::reference_wrapper<int> &&>);
        }

        {
            // std::reference_wrapper<> has a call operator.
            auto f_ref = std::ref(f);

            auto directRes = f_ref();
            auto stdInvokeRes = std::invoke(f_ref);
            auto trInvokeRes = tr::invoke(f_ref);

            static_assert(directRes == stdInvokeRes);
            static_assert(directRes == trInvokeRes);
            static_assert(directRes == tr::type_c<Func &>);
        }

        {
            // std::reference_wrapper<> has a call operator.
            auto f_ref = std::cref(f);

            auto directRes = f_ref();
            auto stdInvokeRes = std::invoke(f_ref);
            auto trInvokeRes = tr::invoke(f_ref);

            static_assert(directRes == stdInvokeRes);
            static_assert(directRes == trInvokeRes);
            static_assert(directRes == tr::type_c<Func const &>);
        }
    }
};
} // namespace