#include <tr/all_of.h>

// -- TODO: I need to include these because they're needed by tr/all_of.h and
// this is counter-intuitive.
#include <tr/at.h>
#include <tr/length.h>
// --

#include <tr/value_constant.h>

using tr::value_c;
using tr::detail::is_complete_v;

namespace {
struct TestAllOf {
    void test() {
        auto isPtrToComplete = [](auto *i) constexpr {
            return value_c<is_complete_v<decltype(*i)>>;
        };

        struct incomplete;
        incomplete *vals[10]{};
        auto ok = tr::all_of(vals, isPtrToComplete);
        static_assert(!ok);
    }
};
} // namespace