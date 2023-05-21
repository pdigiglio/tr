#include <tr/algorithm/all_of.h>

#include <tr/tuple.h>
#include <tr/value_constant.h>
#include <tr/view/drop_view.h>

using tr::all_of;
using tr::tuple;
using tr::value_c;
using tr::detail::is_complete_v;

namespace {

struct incomplete;
struct complete {};

struct Checkable {
    bool Valid_;
};

struct TestAllOf {
    void test() {
        auto isIncomplete = [](auto *i) constexpr {
            return value_c<!is_complete_v<decltype(*i)>>;
        };

        {
            incomplete *incompletes[2]{};
            auto allIncomplete = all_of(incompletes, isIncomplete);
            static_assert(allIncomplete);
        }

        {
            tuple<incomplete *, complete *> someIncomplete{};
            auto allIncomplete = all_of(someIncomplete, isIncomplete);
            static_assert(!allIncomplete);
        }

        static_assert(all_of(tuple{}, [] { /*unevaluated*/ }),
                      "tr::all_of is not true on an empty tuple");

        {
            constexpr Checkable arr[]{false, true, true, true};
            static_assert(!all_of(arr, &Checkable::Valid_));
            static_assert(all_of(arr | tr::drop_c<1>, &Checkable::Valid_));
        }
    }
};
} // namespace