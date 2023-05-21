#include <tr/algorithm/fold_left.h>
#include <tr/algorithm/fold_left_first.h>

#include <tr/tuple.h>
#include <tr/tuple_protocol/std_integer_sequence.h>
#include <tr/value_constant.h>
#include <tr/view/drop_view.h>

#include <functional>
#include <numeric>
#include <string>

namespace {

struct swallow {
    template <typename... Args>
    constexpr void operator()(Args &&...) const noexcept {}
};

struct TestFoldLeft {

    void test() {

        {
            // NOTE:
            //
            // by default, on Clang:
            //
            //  -fbracket-depth=256
            //
            // This option sets the limit for nested parentheses, brackets, and
            // braces.
            //
            // Also note, if you drop_c<N> then you can go up to 256+N with the
            // array below.
            constexpr int values[256]{1, 2, 3, 4};
            constexpr auto sum =
                tr::fold_left(values | tr::drop_c<4>, 0, std::plus{});
            // static_assert(sum == 10);
            static_assert(sum == 0);
        }

        {
            // 12 is the max you can evaluate with 32bit ints.
            auto limit = tr::value_c<12>;
            std::make_integer_sequence<std::int32_t, limit + 1> seq{};
            auto factorial = tr::fold_left(seq | tr::drop_c<1>, tr::value_c<1>,
                                           std::multiplies{});
            static_assert(factorial == 479'001'600);
        }

        {
            using tr::detail::lref_or_value;

            int i{};
            static_assert(std::is_same_v<decltype(lref_or_value(i)), int &>);
            static_assert(
                std::is_same_v<decltype(lref_or_value(std::move(i))), int>);

            int const j{};
            static_assert(
                std::is_same_v<decltype(lref_or_value(j)), int const &>);
            static_assert(
                std::is_same_v<decltype(lref_or_value(std::move(j))), int>);
        }

        {
            using res_t =
                decltype(tr::fold_left(tr::tuple{}, std::string{}, swallow{}));
            static_assert(std::is_same_v<res_t, std::string>);
        }

        {
            std::string s;
            using res_t = decltype(tr::fold_left(tr::tuple{}, s, swallow{}));
            static_assert(std::is_same_v<res_t, std::string &>);
        }

        {
            std::string const s;
            using res_t = decltype(tr::fold_left(tr::tuple{}, s, swallow{}));
            static_assert(std::is_same_v<res_t, std::string const &>);
        }

        {
            struct Accumulator {
                constexpr Accumulator add(int x) const noexcept {
                    return Accumulator{this->Acc_ + x};
                }

                int Acc_;
            };

            constexpr auto res0 = tr::fold_left(
                tr::tuple{1, 2, 3}, Accumulator{}, &Accumulator::add);
            static_assert(res0.Acc_ == 6);

            constexpr auto res1 = tr::fold_left_first(
                tr::tuple{Accumulator{}, 1, 2, 3}, &Accumulator::add);
            static_assert(res1.Acc_ == 6);

            constexpr auto res2 = tr::fold_left(
                tr::tuple{1, 2, 3}, Accumulator{},
                [](Accumulator acc, int i) { return acc.add(i); });
            static_assert(res2.Acc_ == 6);
        }
    }
};
} // namespace
