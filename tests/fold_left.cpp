#include <tr/algorithm/fold_left.h>

#include <tr/lazy_false.h>
#include <tr/tuple.h>
#include <tr/tuple_protocol/std_integer_sequence.h>
#include <tr/value_constant.h>
#include <tr/view/drop_view.h>

#include <functional>

namespace {

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
            constexpr int values[256]{1, 2, 3, 4};
            constexpr auto sum = tr::fold_left(values | tr::drop_c<4>, 0, std::plus{});
            //static_assert(sum == 10);
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
    }
};
} // namespace