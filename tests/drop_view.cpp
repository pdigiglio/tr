#include <tr/view/drop_view.h>

#include <tr/detail/type_traits.h>
#include <tr/detail/utility.h>
#include <tr/tuple_protocol/std_integer_sequence.h>
#include <tr/type_constant.h>
#include <tr/unpack.h>

using tr::detail::check_expr;
using tr::ic;

namespace {
struct TestDropView {
    void test() {

        constexpr std::size_t tupleLength{10};
        using iota_tuple_t = std::make_index_sequence<tupleLength>;

        constexpr std::size_t dropCount{5};
        auto view = iota_tuple_t{} | tr::drop_c<dropCount>;

        static_assert(view.size() == tupleLength - dropCount);

        static_assert(view[ic<0>] == dropCount + 0);
        static_assert(view[ic<1>] == dropCount + 1);
        static_assert(view[ic<2>] == dropCount + 2);
        static_assert(view[ic<3>] == dropCount + 3);
        static_assert(view[ic<4>] == dropCount + 4);

        //constexpr std::integral_constant<std::size_t, tupleLength - dropCount>
        //    outOfBoundIdx{};
        //auto outOfBoundOk =
        //    check_expr([](auto &&tuple, auto i) -> decltype(tuple[i]) {});
        //static_assert(!outOfBoundOk(view, outOfBoundIdx));

        using expected_drop_idx_seq_t = std::index_sequence<5, 6, 7, 8, 9>;
        auto dropIdxSeq = tr::unpack(
            view, [](auto... is) { return std::index_sequence<is...>{}; });

        static_assert(tr::type_c<decltype(dropIdxSeq)> ==
                      tr::type_c<expected_drop_idx_seq_t>);


        //{
        //    auto checkDropExpr = check_expr(
        //        [](auto &&tuple, auto dropCount)
        //            -> decltype(std::forward<decltype(tuple)>(tuple) |
        //                        tr::drop_c<decltype(dropCount)::value>) {});

        //    int a[3]{};
        //    static_assert(checkDropExpr(a, tr::value_c<3>));
        //    static_assert(!checkDropExpr(a, tr::value_c<4>));
        //}
    }
};
} // namespace
