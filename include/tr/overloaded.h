#pragma once

namespace tr {

namespace detail {
template <typename Callable>
struct CallableWrapper : Callable {};

template <typename R, typename... Args>
struct CallableWrapper<R (*)(Args...)> {
    R (*Func_)(Args...);

    constexpr R operator()(Args... args) const {
        return this->Func_(static_cast<Args>(args)...);
    }
};

} // namespace detail

/// @brief A class to compose a number of `Callables...` and provide an
/// overloaded call operator.
/// 
/// @details The call-operator overload set is the union of the overload sets in
/// each of the `Callables...`, e.g.:
/// 
/// @code
/// int f(int);
/// 
/// // o0 has two call-operator overloads (i.e. for int, double)
/// overloaded o0 {f, [](double) { /* ... */ }};
/// 
/// // o1 has three call-operator overloads (i.e. for int, double, char const*)
/// overloaded o1 {o0, [](char const*) { /* ... */ }};
/// @endcode
///
/// @tparam ...Callables The callable types.
template <typename... Callables>
struct overloaded : detail::CallableWrapper<Callables>... {
    using detail::CallableWrapper<Callables>::operator()...;
};

template <typename... Callables>
overloaded(Callables...) -> overloaded<Callables...>;

} // namespace tr