#include <tr/all_of.h>

// -- TODO: I need to include these because they're needed by tr/all_of.h and
// this is counter-intuitive.
#include <tr/at.h>
#include <tr/length.h>
// --

#include <tr/as_array.h>
#include <tr/tuple.h>
#include <tr/value_constant.h>

using tr::all_of;
using tr::as_array;
using tr::tuple;
using tr::value_c;
using tr::detail::is_complete_v;

namespace {

struct incomplete;
struct complete {};

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
    }
};
} // namespace