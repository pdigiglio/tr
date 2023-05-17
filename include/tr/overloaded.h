#pragma once

#include <tr/macros.h>

namespace tr {

namespace detail {

/// @brief Primary template.
/// @tparam Callable A class type with a call operator.
template <typename Callable>
struct CallableWrapper : Callable {};

/// @brief Specialization for function pointers.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
template <typename R, typename... Args>
struct CallableWrapper<R (*)(Args...)> {
    R (*Func_)(Args...);

    constexpr auto operator()(Args... args) const -> R {
        return this->Func_(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to non-const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...)> {
    R (C::*Func_)(Args...);

    constexpr auto operator()(C &c, Args... args) const /* noexcept(?) */ -> R {
        return (c.*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C &&c, Args... args) const
        /* noexcept(?) */ -> R {
        return (static_cast<C &&>(c).*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C *c, Args... args) const /* noexcept(?) */ -> R {
        return (c->*this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function arguments types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...) const> {
    R (C::*Func_)(Args...) const;

    constexpr auto operator()(C const &c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c.*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C const &&c, Args... args) const
        /* noexcept(?) */ -> R {
        return (static_cast<C &&>(c).*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C const *c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c->*this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to non-const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...) &> {
    R (C::*Func_)(Args...) &;

    constexpr auto operator()(C &c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c.*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C *c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c->*this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...) const &> {
    R (C::*Func_)(Args...) const &;

    constexpr auto operator()(C const &c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c.*this->Func_)(static_cast<Args>(args)...);
    }

    constexpr auto operator()(C const *c, Args... args) const
        /* noexcept(?) */ -> R {
        return (c->*this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to non-const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...) &&> {
    R (C::*Func_)(Args...) &&;

    constexpr auto operator()(C &&c, Args... args) const
        /* noexcept(?) */ -> R {
        return (static_cast<C &&>(c).*this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization for pointer to const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct CallableWrapper<R (C::*)(Args...) const &&> {
    R (C::*Func_)(Args...) const &&;

    constexpr auto operator()(C const &&c, Args... args) const
        /* noexcept(?) */ -> R {
        return (static_cast<C const &&>(c).*
                this->Func_)(static_cast<Args>(args)...);
    }
};

/// @brief Specialization to pointer to member variable.
/// @tparam R The member variable type.
/// @tparam C The class type.
template <typename R, typename C>
struct CallableWrapper<R C::*> {
    R C::*MemPtr_;

    constexpr auto operator()(C &c) const
        /* noexcept(?) */ -> R & {
        return (c.*this->MemPtr_);
    }

    constexpr auto operator()(C const &c) const
        /* noexcept(?) */ -> R const & {
        return (c.*this->MemPtr_);
    }

    constexpr auto operator()(C &&c) const
        /* noexcept(?) */ -> R && {
        return (static_cast<C &&>(c).*this->MemPtr_);
    }

    constexpr auto operator()(C const &&c) const
        /* noexcept(?) */ -> R const && {
        return (static_cast<C const &&>(c).*this->MemPtr_);
    }

    constexpr auto operator()(C *c) const
        /* noexcept(?) */ -> R & {
        return (c->*this->MemPtr_);
    }

    constexpr auto operator()(C const *c) const
        /* noexcept(?) */ -> R const & {
        return (c->*this->MemPtr_);
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
struct TR_EMPTY_BASES overloaded : detail::CallableWrapper<Callables>... {
    using detail::CallableWrapper<Callables>::operator()...;
};

template <typename... Callables>
overloaded(Callables...) -> overloaded<Callables...>;

} // namespace tr