#include <tr/overload.h>

namespace {

template <typename T>
void foo(T) noexcept {}

void f(int) {}
void f(double) {}
void f(char) {}

struct S {

    void f(int) {}
    void f(double) && {}
    void f(char) const & {}
};

struct TestOverload {
    void test() {

        {
            // Instanciate function templates.
            auto f_may_throw = tr::overload<void(int)>(foo);
            static_assert(!noexcept(f_may_throw(int{})));

            auto f_noexcept = tr::overload<void(int) noexcept>(foo);
            static_assert(noexcept(f_noexcept(int{})));
        }

        {
            // Select a free function from an overload set.
            void (*f_char)(char) = f;
            f_char = tr::overload<void(char)>(f);
            (void)f_char;

            void (*f_int)(int) = f;
            f_int = tr::overload<void(int)>(f);
            (void)f_int;

            void (*f_double)(double) = f;
            f_double = tr::overload<void(double)>(f);
            (void)f_double;
        }

        {
            void (S::*f_mutable)(int) = &S::f;
            f_mutable = tr::overload<void(int)>(&S::f);
            (void)f_mutable;

            void (S::*f_mutable_rref)(double) && = &S::f;
            f_mutable_rref = tr::overload<void(double) &&>(&S::f);
            (void)f_mutable_rref;

            void (S::*f_const_lref)(char) const & = &S::f;
            f_const_lref = tr::overload<void(char) const &>(&S::f);
            (void)f_const_lref;
        }
    }
};

} // namespace