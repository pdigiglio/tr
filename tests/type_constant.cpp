#include <tr/type_constant.h>

using tr::type_c;
using tr::type_constant;

namespace {
struct TestValueConstant {
    void test() {
        static_assert(type_c<int> == type_c<int>);
        static_assert(type_c<int> != type_c<int &>);
    }
};
} // namespace