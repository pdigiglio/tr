#pragma once

#include <tr/detail/callable_wrapper_impl.h>
#include <tr/fwd/type_pack.h>
#include <tr/macros.h>

namespace tr {

namespace detail {

/// @brief Primary template.
/// @tparam Callable A class type with a call operator.
template <typename Callable>
struct overload_elem : Callable {};

/// @brief Specialization for function pointers.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
template <typename R, typename... Args>
struct overload_elem<R (*)(Args...)>
    : fptr_wrapper<R (*)(Args...), false, R, type_pack<Args...>> {};

/// @brief Specialization for `noexcept` function pointers.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
template <typename R, typename... Args>
struct overload_elem<R (*)(Args...) noexcept>
    : fptr_wrapper<R (*)(Args...) noexcept, true, R, type_pack<Args...>> {};

/// @brief Specialization for pointer to non-const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...)>
    : mem_fptr_wrapper<R (C::*)(Args...), false, type_pack<C &, C &&, C *>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to `noexcept` non-const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) noexcept, true,
                       type_pack<C &, C &&, C *>, R, type_pack<Args...>> {};

/// @brief Specialization for pointer to const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function arguments types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const>
    : mem_fptr_wrapper<R (C::*)(Args...) const, false,
                       type_pack<C const &, C const &&, C const *>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to `noexcept` const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function arguments types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const noexcept, true,
                       type_pack<C const &, C const &&, C const *>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to non-const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) &>
    : mem_fptr_wrapper<R (C::*)(Args...) &, false, type_pack<C &, C *>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to `noexcept` non-const l-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) &noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) &noexcept, true, type_pack<C &, C *>,
                       R, type_pack<Args...>> {};

/// @brief Specialization for pointer to const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const &>
    : mem_fptr_wrapper<R (C::*)(Args...) const &, false,
                       type_pack<C const &, C const *>, R, type_pack<Args...>> {
};

/// @brief Specialization for pointer to `noexcept` const l-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const &noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const &noexcept, true,
                       type_pack<C const &, C const *>, R, type_pack<Args...>> {
};

/// @brief Specialization for pointer to non-const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) &&>
    : mem_fptr_wrapper<R (C::*)(Args...) &&, false, type_pack<C &&>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to `noexcept` non-const r-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) &&noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) &&noexcept, true, type_pack<C &&>, R,
                       type_pack<Args...>> {};

/// @brief Specialization for pointer to const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const &&>
    : mem_fptr_wrapper<R (C::*)(Args...) const &&, false, type_pack<C const &&>,
                       R, type_pack<Args...>> {};

/// @brief Specialization for pointer to `noexcept` const r-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct overload_elem<R (C::*)(Args...) const &&noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const &&noexcept, true,
                       type_pack<C const &&>, R, type_pack<Args...>> {};

/// @brief Specialization to pointer to member variable.
/// @tparam R The member variable type.
/// @tparam C The class type.
template <typename R, typename C>
struct overload_elem<R C::*> : mem_ptr_wrapper<R C::*> {};

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
struct TR_EMPTY_BASES overloaded : detail::overload_elem<Callables>... {
    using detail::overload_elem<Callables>::operator()...;
};

template <typename... Callables>
overloaded(Callables...) -> overloaded<Callables...>;

} // namespace tr