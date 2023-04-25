#include <tr/tuple.h>

#include <tr/as_array.h>
#include <tr/at.h>
#include <tr/lazy_false.h>
#include <tr/length.h>
#include <tr/type_constant.h>
#include <tr/value_constant.h>

#include <string>
#include <tuple>

using tr::false_c;
using tr::true_c;
using tr::type_c;
using tr::value_c;

using namespace tr::literals;

namespace {

template <typename T>
void td(){ nullptr = tr::lazy_false<T>; }

template <typename T, std::size_t N, std::size_t... Is>
constexpr bool array_equal_impl(T const (&lhs)[N], T const (&rhs)[N],
                                std::index_sequence<Is...>) {
    using tr::at_c;
    return ((at_c<Is>(lhs) == at_c<Is>(rhs)) && ...);
}

template <typename T, std::size_t N>
constexpr bool array_equal(T const (&lhs)[N], T const (&rhs)[N]) {
    return array_equal_impl(lhs, rhs, std::make_index_sequence<N>{});
}

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

using tr::detail::same_extents_v;
static_assert(same_extents_v<int, double>);
static_assert(!same_extents_v<int[2], double>);
static_assert(!same_extents_v<int[2], double[1]>);
static_assert(same_extents_v<int[2], double[2]>);
static_assert(!same_extents_v<int[2][2], double[2]>);
static_assert(!same_extents_v<int[2][2], double[2][1]>);
static_assert(same_extents_v<int[2][2], double[2][2]>);

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
            static_assert(
                !std::is_swappable_with_v<tr::tuple<SwapDeleted> &,
                                          tr::tuple<SwapDeleted &> &>);
        }

        {
            // NotMoveable is not swappable because the default std::swap
            // implementation requires its argument to be move-assignable.
            static_assert(!std::is_swappable_v<NotMoveable>);
            static_assert(!std::is_swappable_v<tr::tuple<NotMoveable>>);
            static_assert(
                !std::is_swappable_with_v<tr::tuple<NotMoveable> &,
                                          tr::tuple<NotMoveable &> &>);
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

        {
            using tr::detail::check_expr;
            using tr::detail::tup_elem;

            auto assignment_valid = check_expr(
                [](auto &&lhs, auto &&rhs)
                    -> decltype(static_cast<decltype(lhs)>(lhs) =
                                    static_cast<decltype(rhs)>(rhs)) {});

            tup_elem<int[3], 0> to{};
            static_assert(assignment_valid(to, to));
            static_assert(!assignment_valid(std::as_const(to), to));
            static_assert(assignment_valid(to, std::move(to)));
            // Should this compile? Does assigning to a temporary make sense?
            static_assert(assignment_valid(std::move(to), to));

            int i[3]{0, 1, 2};
            tup_elem<int(&)[3], 0> from_i_ref{i};
            static_assert(assignment_valid(from_i_ref, from_i_ref));
            static_assert(
                !assignment_valid(std::as_const(from_i_ref), from_i_ref));
            static_assert(assignment_valid(from_i_ref, std::move(from_i_ref)));
            // Should this compile? Does assigning to a temporary make sense?
            static_assert(assignment_valid(std::move(from_i_ref), from_i_ref));

            static_assert(assignment_valid(to, from_i_ref));
            static_assert(assignment_valid(to, std::move(from_i_ref)));

            double d[3]{};
            tup_elem<double(&)[3], 0> from_d_ref{d};
            static_assert(assignment_valid(to, from_d_ref));
            static_assert(assignment_valid(to, std::move(from_d_ref)));

            tup_elem<double[3], 0> from_d{d};
            static_assert(assignment_valid(to, from_d));
            static_assert(assignment_valid(to, std::move(from_d)));
        }

        // TODO: make this compile.
        {
            // TODO: Assert that assigning two tuples of different sizes doesn't
            // compile.
            //
            // TODO: Should I allow assignment from other tuple-like objects?
            // Or should I delegate this to an `assign` or `copy` algorithm?
            // (std::tuple is only assignable from std::pair, other than
            // std::tuple).
            //
            // NOTE: If I allow tuple-like assignment, then it's not clear what
            // this does:
            //
            //   int a[]{0,1,2};
            //   tr::tuple t{a}; // tuple<int[3]> or tuple<int,int,int> ?
            //
            // Probably, I should only allow assignment from std::tuple (and
            // provide a tr::to_tuple which is analogous to std::to_array).

            using tuple0_t = tr::tuple<int, double>;
            using tuple1_t = tr::tuple<int &, double &>;

            {
                int i{};
                double d{};

                tuple0_t lhs{};
                lhs = tuple1_t{i, d};
            }

            {
                int i{};
                double d{};
                tuple1_t tie{i, d};
                tie = tuple0_t{};
            }
        }

        {

            using tr::detail::tup_elem_base;

            int i{};

            //tup_elem_base<int &&, 0> teb{std::move(i)};
            // static_assert(std::is_same_v<decltype(teb.value()), int &&>);
            //td<decltype(teb.value())>();

            //tr::tuple<int &&> t{1};
            //t = tr::tuple{i};

            // NOTE: this is UB.
            std::tuple<int &&> st{1};
            st = std::tuple{i};
        }

        {
            int i{};

            std::tuple<int &> st_ref{i};
            std::tuple<int> st_val{st_ref};

            static_assert(std::is_constructible_v<
                          int, decltype(std::declval<
                                        tr::tuple<int &> >()[0_c])>);

            tr::tuple<int &> t_ref{i};
            [[maybe_unused]] tr::tuple<int> t_val = t_ref;
        }

        {
            //static_assert(std::is_constructible_v<int &, int>);
            tr::tuple<int> t_val{};
            [[maybe_unused]] tr::tuple<int &> t_ref = t_val;
        }

        {
            std::tuple<int &&> st_rref{1};
            std::tuple<int> st{st_rref};

            static_assert(std::is_constructible_v<int, int &&>);

            tr::tuple<int &&> t_rref{1};

            //td<decltype(t_rref[zuic<0>])>();
            //td<decltype(t_rref.value())>();

            [[maybe_unused]] tr::tuple<int> t = t_rref;
        }

        {
            tr::tuple<int[2]> t{};
            [[maybe_unused]] tr::tuple<int(&)[2]> t_lref = t;
            [[maybe_unused]] tr::tuple<int const(&)[2]> t_lcref = t;

            static_assert(
                std::is_same_v<decltype(std::move(t).value()), int(&&)[2]>);
            [[maybe_unused]] tr::tuple<int(&&)[2]> t_rref = std::move(t);
            [[maybe_unused]] tr::tuple<int const(&&)[2]> t_rcref = std::move(t);
        }

        {
            int a[]{0, 1};
            tr::tuple<int(&)[2]> t_lref{a};
            [[maybe_unused]] tr::tuple<int[2]> t0 = t_lref;

            tr::tuple<int const(&)[2]> t_lcref{a};
            [[maybe_unused]] tr::tuple<int[2]> t1 = t_lcref;

            tr::tuple<int(&&)[2]> t_rref{std::move(a)};
            [[maybe_unused]] tr::tuple<int[2]> t2 = t_rref;

            tr::tuple<int const(&&)[2]> t_rcref{std::move(a)};
            [[maybe_unused]] tr::tuple<int[2]> t3 = t_rcref;
        }

        {
            int i{};
            tr::tuple<int &> t_ref{i};
            [[maybe_unused]] tr::tuple<int &> t_val{t_ref};
        }
    }

    void test_triviality() {
        struct empty {};

        {
            using tuple_t = tr::tuple<int[4], empty>;
            static_assert(std::is_nothrow_copy_assignable_v<tuple_t>);
            static_assert(std::is_standard_layout_v<tuple_t>);
            static_assert(std::is_trivially_constructible_v<tuple_t>);
            static_assert(std::is_trivially_copyable_v<tuple_t>);
            static_assert(std::is_trivially_default_constructible_v<tuple_t>);
            static_assert(std::is_trivially_destructible_v<tuple_t>);
        }

        {
            using tuple_t = tr::tuple<int, empty>;
            static_assert(std::is_nothrow_copy_assignable_v<tuple_t>);
            static_assert(std::is_standard_layout_v<tuple_t>);
            static_assert(std::is_trivially_constructible_v<tuple_t>);
            static_assert(std::is_trivially_copyable_v<tuple_t>);
            static_assert(std::is_trivially_default_constructible_v<tuple_t>);
            static_assert(std::is_trivially_destructible_v<tuple_t>);
        }

        {
            using tuple_t = tr::tuple<int&, empty>;
            static_assert(std::is_nothrow_copy_assignable_v<tuple_t>);
            static_assert(!std::is_standard_layout_v<tuple_t>);
            static_assert(!std::is_trivially_constructible_v<tuple_t>);
            static_assert(!std::is_trivially_copyable_v<tuple_t>);
            static_assert(!std::is_trivially_default_constructible_v<tuple_t>);
            static_assert(std::is_trivially_destructible_v<tuple_t>);
        }

        {
            using tuple_t = tr::tuple<int, std::string>;
            static_assert(!std::is_nothrow_copy_assignable_v<tuple_t>);
            static_assert(!std::is_standard_layout_v<tuple_t>);
            static_assert(!std::is_trivially_constructible_v<tuple_t>);
            static_assert(!std::is_trivially_copyable_v<tuple_t>);
            static_assert(!std::is_trivially_default_constructible_v<tuple_t>);
            static_assert(!std::is_trivially_destructible_v<tuple_t>);
        }
    }

    void test_at() {
        using tr::tuple, tr::at;

        constexpr char str[] = "hello";
        constexpr tuple t{0, 1, str};
        static_assert(at(t, 0_c) == t[0_c]);
        static_assert(at(t, 1_c) == t[1_c]);

        // Compare addresses
        static_assert(at(t, 2_c) == &t[2_c][0]);
        // Compare elements
        static_assert(array_equal(at(t, 2_c), t[2_c]));
        static_assert(array_equal(at(t, 2_c), str));

        using tr::at_c;
        static_assert(at_c<0>(t) == t[0_c]);
        static_assert(at_c<1>(t) == t[1_c]);
        static_assert(array_equal(at_c<2>(t), str));
    }

    void test_length() {
        using tr::tuple, tr::length;

        tuple empty{};
        static_assert(length(empty) == 0);

        tuple oneElem{0};
        static_assert(length(oneElem) == 1);

        tuple twoElems{0, tuple{}};
        static_assert(length(twoElems) == 2);

        int fourElems[]{0, 1, 2, 3};
        static_assert(length(fourElems) == 4);
    }
};
} // namespace
