#pragma once

#include <type_traits>
#include <utility>

namespace tr {

namespace detail {
template <typename Callable>
struct CallableWrapper : Callable {};

template <typename R, typename... Args>
struct CallableWrapper<R (*)(Args...)> {
    R (*Func_)(Args...);

    constexpr R operator()(Args... args) const {
        return this->Func_(std::forward<Args>(args)...);
    }
};
} // namespace detail

template <typename... Callables>
struct overload : detail::CallableWrapper<Callables>... {
    using detail::CallableWrapper<Callables>::operator()...;
};

template <typename... Callables>
overload(Callables &&...) -> overload<std::decay_t<Callables>...>;

} // namespace tr