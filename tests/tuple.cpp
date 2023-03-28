#include <tr/tuple.h>
#include <tr/type_constant.h>
#include <tr/value_constant.h>

#include <string>
#include <tuple>

using tr::false_c;
using tr::true_c;
using tr::type_c;
using tr::value_c;

namespace {

struct SwapDeleted {
    friend auto swap(SwapDeleted &, SwapDeleted &) -> void = delete;
};

struct NotMoveable {
    NotMoveable(NotMoveable &&) = delete;
};

template <typename... Ts, template <typename...> typename Tup, typename... Us>
constexpr auto deduced_types_equal(Tup<Us...>) noexcept {
    return value_c<((type_c<Ts> == type_c<Us>)&&... && true)>;
}

template <typename... Ts>
constexpr auto same_args(std::tuple<Ts...>, tr::tuple<Ts...>) noexcept {
    return true_c;
}

template <typename StdTup, typename TrTup>
constexpr auto same_args(StdTup, TrTup) noexcept {
    return false_c;
}

#define ASSERT_CTAD_SAME(EXPECTED, ...)                                        \
    (                                                                          \
        [&] {                                                                  \
            std::tuple std_tuple{__VA_ARGS__};                                 \
            tr::tuple tr_tuple{__VA_ARGS__};                                   \
            auto sameArgs = same_args(std_tuple, tr_tuple);                    \
            static_assert(sameArgs == EXPECTED,                                \
                          "Deduced class templates differ");                   \
        }(),                                                                   \
        (void)0)

struct TestTuple {
    void test_ctad() {
        ASSERT_CTAD_SAME(true_c, 0, 1.2, 'a');

        {
            int i{};
            ASSERT_CTAD_SAME(true_c, i, 1.2, 'a');
        }

        {

            int const ci{};
            ASSERT_CTAD_SAME(true_c, ci, 1.2, 'a');
        }

        {
            int i{};
            ASSERT_CTAD_SAME(false_c, i, 1.2, "str");
            same_args(std::tuple<int, double, char[4]>{},
                      tr::tuple{i, 1.2, "str"});
            same_args(std::tuple{i, 1.2, "str"},
                      tr::tuple<int, double, char const *>{});
        }

        {
            using tr::as_array;
            using tr::ic;

            {
                // Make sure I can construct and (kind-of) aggregate-initialize
                // a tuple with an element of array type.
                constexpr tr::tuple<int, int[3]> t{0, {}};

                // Make sure the array is value-initialized.
                static_assert(t[ic<1>][0] == 0);
                static_assert(t[ic<1>][1] == 0);
                static_assert(t[ic<1>][2] == 0);
            }

            // Make sure the construction works fine with multi-dimensional
            // arrays too.
            (void)tr::tuple<int[3][2]>{tr::as_array<int[][2]>{{}}};

            // I want `tuple<T[N]>` to be constructible from `T(&)[N]`.
            constexpr int arr[]{0, 1, 2};
            constexpr tr::tuple<int[3]> t0{arr};
            static_assert(t0[ic<0>][0] == 0);
            static_assert(t0[ic<0>][1] == 1);
            static_assert(t0[ic<0>][2] == 2);

            // For `tuple<T[N]>` to be constructible from `T(&)[N]`, I need a
            // user-defined constructor. So I loose the possibility for aggre-
            // gate initialization.
            //
            // Whenever I want to build an element of array type, I must cast
            // the initializer list with `as_array`.
            constexpr tr::tuple<int, int[3]> t1{0, as_array<int[]>{1}};
            static_assert(t1[ic<1>][0] == 1);
            static_assert(t1[ic<1>][1] == 0);
            static_assert(t1[ic<1>][2] == 0);

            static_assert(
                deduced_types_equal<int[3]>(tr::tuple{as_array<int[3]>{}}));
            static_assert(deduced_types_equal<int[3]>(tr::tuple{arr}));
        }

        {
            // Make sure tuple with no type argument compiles
            (void)tr::tuple<>{};
        }
    }

    void test_swap() {
        using std::swap;

        {
            tr::tuple lhs{0, "hello", 0.2};
            static_assert(decltype(deduced_types_equal<int, char[6], double>(
                lhs))::value);

            int i{};
            double d{};
            char buffer[6]{};
            auto rhs = tr::tie(i, buffer, d);
            static_assert(
                decltype(deduced_types_equal<int &, char(&)[6], double &>(
                    rhs))::value);

            lhs.swap(rhs);
            rhs.swap(lhs);

            swap(lhs, rhs);
            swap(rhs, lhs);
            static_assert(
                std::is_swappable_with_v<decltype(rhs) &, decltype(rhs) &>);

            swap(lhs, lhs);
            static_assert(std::is_swappable_v<decltype(lhs)>);

            swap(rhs, rhs);
            static_assert(std::is_swappable_v<decltype(rhs)>);
        }

        {
            // SwapDeleted is not swappable because I explicitly introduced a
            // deleted swap() overload.
            static_assert(!std::is_swappable_v<SwapDeleted>);
            static_assert(!std::is_swappable_v<tr::tuple<SwapDeleted>>);
            static_assert(!std::is_swappable_with_v<tr::tuple<SwapDeleted>&, tr::tuple<SwapDeleted&>&>);
        }

        {
            // NotMoveable is not swappable because the default std::swap
            // implementation requires its argument to be move-assignable.
            static_assert(!std::is_swappable_v<NotMoveable >);
            static_assert(!std::is_swappable_v<tr::tuple<NotMoveable>>);
            static_assert(!std::is_swappable_with_v<tr::tuple<NotMoveable>&, tr::tuple<NotMoveable&>&>);
        }

        {
            tr::tuple<> lhs, rhs;
            static_assert(std::is_swappable_v<tr::tuple<>>);
            lhs.swap(rhs);
            swap(lhs, rhs);
        }
    }

    void test_aggregateness() {
        using std::is_aggregate_v;
        using tr::tuple;

        static_assert(is_aggregate_v<tuple<>>);
        static_assert(is_aggregate_v<tuple<int>>);

        // tuple is aggregate even if some of its member variables are not
        static_assert(!is_aggregate_v<std::string>);
        static_assert(is_aggregate_v<tuple<std::string>>);
        static_assert(is_aggregate_v<tuple<std::string, int>>);
        static_assert(is_aggregate_v<tuple<std::string, int &>>);

        static_assert(is_aggregate_v<tuple<int[3]>>);
        static_assert(is_aggregate_v<tuple<int[3][2], double>>);
    }

    void test_assignment() {
        {
            using tuple_t = tr::tuple<int, double, std::string>;
            tuple_t lhs{};
            tuple_t const rhs{};
            lhs = rhs;
            lhs = tuple_t{};
        }

        // TODO: make this compile.
        //{
        //    using tuple0_t = tr::tuple<int, double>;
        //    using tuple1_t = tr::tuple<int&, double&>;

        //    {
        //        int i{};
        //        double d{};

        //        tuple0_t lhs{};
        //        lhs = tuple1_t{i, d};
        //    }

        //    {
        //        int i{};
        //        double d{};
        //        tuple1_t tie{i, d};
        //        tie = tuple0_t{};
        //    }
        //}
    }
};
} // namespace