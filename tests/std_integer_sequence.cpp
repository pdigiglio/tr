#include <tr/tuple_protocol/std_integer_sequence.h>

#include <tr/at.h>
#include <tr/detail/utility.h>
#include <tr/length.h>
#include <tr/value_constant.h>

using tr::value_c;
using tr::zuic;

namespace {
struct TestStdIntegerSequence {

    template <typename T, T... Vals, std::size_t... Is>
    void test_vals_impl(std::integer_sequence<T, Vals...> seq,
                        std::index_sequence<Is...>) {
        using tr::at;
        static_assert(((at(seq, zuic<Is>) == Vals) && ...));

        using tr::at_c;
        static_assert(((at_c<Is>(seq) == Vals) && ...));
    }

    template <typename T, T... Vals>
    void test_vals(std::integer_sequence<T, Vals...> seq) {
        test_vals_impl(seq, std::make_index_sequence<sizeof...(Vals)>{});
    }

    void test() {
        auto seq = std::make_index_sequence<10>{};
        test_vals(seq);

        auto hello = std::integer_sequence<char, 'h', 'e', 'l', 'l', 'o'>{};
        test_vals(hello);

        using tr::length;
        static_assert(length(seq) == 10);
        static_assert(length(hello) == 5);
    }
};
} // namespace