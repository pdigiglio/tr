#include <tr/value_sequence.h>

#include <tr/detail/utility.h>
#include <tr/type_constant.h>

#include <tuple>

using tr::array_c;
using tr::ic;
using tr::tuple_c;
using tr::type_c;

namespace {

struct TestValueSequence {
    void test() {
        {
            auto arr = array_c<int, 0, 2, 3>;
            static_assert(arr[ic<0>] == 0);
            static_assert(arr[ic<1>] == 2);
            static_assert(arr[ic<2>] == 3);

            auto [a0, a1, a2] = arr;
            static_assert(decltype(a0){} == 0);
            static_assert(decltype(a1){} == 2);
            static_assert(decltype(a2){} == 3);
        }

        {
            auto tup = tuple_c<'0', 1l, 2u>;
            static_assert(tup[ic<0>] == '0');
            static_assert(tup[ic<1>] == 1l);
            static_assert(tup[ic<2>] == 2u);

            auto [t0, t1, t2] = tup;
            static_assert(type_c<decltype(t0)::value_type> == type_c<char>);
            static_assert(decltype(t0){} == '0');

            static_assert(type_c<decltype(t1)::value_type> == type_c<long>);
            static_assert(decltype(t1){} == 1);

            static_assert(type_c<decltype(t2)::value_type> == type_c<unsigned>);
            static_assert(decltype(t2){} == 2);
        }
    }
};

} // namespace