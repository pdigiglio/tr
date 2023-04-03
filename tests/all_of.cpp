#include <tr/all_of.h>

// -- TODO: I need to include these because they're needed by tr/all_of.h and
// this is counter-intuitive.
#include <tr/at.h>
#include <tr/length.h>
// --

// -- TODO: value_constant needs operators. And logical operators should
// short-circuit.
#include <tr/value_constant.h>

namespace tr {

template <auto Val0, auto Val1>
constexpr auto operator&&(value_constant<Val0>, value_constant<Val1>) noexcept
    -> value_constant<Val0 && Val1> {
    return {};
}
} // namespace tr
// --

namespace {
struct TestAllOf {
    void test() {
        auto isPtrToComplete = [](auto *i) constexpr {
            using tr::detail::is_complete_v;
            return tr::value_c<is_complete_v<decltype(*i)>>;
        };

        struct incomplete;
        incomplete *vals[10]{};
        auto ok = tr::all_of(vals, isPtrToComplete);
        static_assert(!ok);
    }
};
} // namespace