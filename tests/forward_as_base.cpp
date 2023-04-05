#include <tr/forward_as_base.h>

#include <tr/type_constant.h>

using tr::forward_as_base;
using tr::type_c;

namespace {

struct Base {};
struct Derived : Base {};

struct TestForwardAs {
    void test() {
        Derived d;
        static_assert(type_c<decltype(forward_as_base<Base, Derived &>(d))> ==
                      type_c<Base &>);

        static_assert(type_c<decltype(forward_as_base<Base, Derived const &>(
                          std::as_const(d)))> == type_c<Base const &>);

        static_assert(
            type_c<decltype(forward_as_base<Base, Derived &&>(std::move(d)))> ==
            type_c<Base &&>);

        static_assert(type_c<decltype(forward_as_base<Base, Derived const &&>(
                          std::move(std::as_const(d))))> ==
                      type_c<Base const &&>);
    }
};
} // namespace