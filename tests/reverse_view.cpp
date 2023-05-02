#include <tr/view/reverse_view.h>

#include <tr/algorithm/all_of.h>
#include <tr/tuple_protocol/std_integer_sequence.h>

using tr::reverse;

namespace {

template <typename Tuple0, typename Tuple1>
constexpr bool equal(Tuple0 &&lhs, Tuple1 &&rhs) {
    using tr::length, tr::all_of, tr::at;
    auto indices = tr::indices_for(lhs);
    return (length(lhs) == length(rhs)) &&
           all_of(indices, [&](auto i) { return at(lhs, i) == at(rhs, i); });
}

struct TestReverseView {
    void test() {

        constexpr int fw[]{1, 2, 3, 4, 5};
        constexpr int rv[]{5, 4, 3, 2, 1};

        static_assert(equal(fw | reverse, rv));
        static_assert(equal(fw, rv | reverse));

        // Reversing twice doesn't do anything.
        static_assert(equal(fw | reverse | reverse, fw));
        static_assert(equal(rv | reverse | reverse, rv));
    }
};
} // namespace