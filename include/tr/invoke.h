#pragma once

#include <tr/fwd/type_pack.h>

#include <tr/detail/callable_wrapper_impl.h>

#include <type_traits>

namespace tr {
namespace detail {

/// @brief Primary template.
/// @tparam Callable A class type with a call operator.
template <typename Callable>
struct invoke_wrapper : Callable {};

/// @brief Specialization for function pointers.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
template <typename R, typename... Args>
struct invoke_wrapper<R (*)(Args...)> : fptr_wrapper<R (*)(Args...), false, R> {
};

/// @brief Specialization for `noexcept` function pointers.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
template <typename R, typename... Args>
struct invoke_wrapper<R (*)(Args...) noexcept>
    : fptr_wrapper<R (*)(Args...) noexcept, true, R> {};

/// @brief Specialization for pointer to non-const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...)>
    : mem_fptr_wrapper<R (C::*)(Args...), false, type_pack<C &, C &&, C *>, R> {
};

/// @brief Specialization for pointer to `noexcept` non-const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) noexcept, true,
                       type_pack<C &, C &&, C *>, R> {};

/// @brief Specialization for pointer to const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function arguments types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const>
    : mem_fptr_wrapper<R (C::*)(Args...) const, false,
                       type_pack<C const &, C const &&, C const *>, R> {};

/// @brief Specialization for pointer to `noexcept` const member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function arguments types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const noexcept, true,
                       type_pack<C const &, C const &&, C const *>, R> {};

/// @brief Specialization for pointer to non-const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) &>
    : mem_fptr_wrapper<R (C::*)(Args...) &, false, type_pack<C &, C *>, R> {};

/// @brief Specialization for pointer to `noexcept` non-const l-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) &noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) &noexcept, true, type_pack<C &, C *>,
                       R> {};

/// @brief Specialization for pointer to const l-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const &>
    : mem_fptr_wrapper<R (C::*)(Args...) const &, false,
                       type_pack<C const &, C const *>, R> {};

/// @brief Specialization for pointer to `noexcept` const l-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const &noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const &noexcept, true,
                       type_pack<C const &, C const *>, R> {};

/// @brief Specialization for pointer to non-const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) &&>
    : mem_fptr_wrapper<R (C::*)(Args...) &&, false, type_pack<C &&>, R> {};

/// @brief Specialization for pointer to `noexcept` non-const r-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) &&noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) &&noexcept, true, type_pack<C &&>, R> {
};

/// @brief Specialization for pointer to const r-value member functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const &&>
    : mem_fptr_wrapper<R (C::*)(Args...) const &&, false, type_pack<C const &&>,
                       R> {};

/// @brief Specialization for pointer to `noexcept` const r-value member
/// functions.
/// @tparam R The function return type.
/// @tparam ...Args The function argument types.
/// @tparam C The class type.
template <typename R, typename C, typename... Args>
struct invoke_wrapper<R (C::*)(Args...) const &&noexcept>
    : mem_fptr_wrapper<R (C::*)(Args...) const &&noexcept, true,
                       type_pack<C const &&>, R> {};

/// @brief Specialization to pointer to member variable.
/// @tparam R The member variable type.
/// @tparam C The class type.
template <typename R, typename C>
struct invoke_wrapper<R C::*> : mem_ptr_wrapper<R C::*> {};

template <typename Func>
struct callable_obj_wrapper {

  private:
    template <typename... Args>
    static constexpr bool is_nothrow{
        noexcept(std::declval<Func>()(std::declval<Args>()...))};

    template <typename... Args>
    using return_t = decltype(std::declval<Func>()(std::declval<Args>()...));

    static_assert(std::is_reference_v<Func>, "Func should be a reference type");

  public:
    Func Func_;

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const
        noexcept(is_nothrow<Args &&...>) -> return_t<Args &&...> {
        return static_cast<Func>(this->Func_)(static_cast<Args &&>(args)...);
    }
};

/// @brief Create an `invoke_wrapper` for `func`. This function is meant to be
/// for internal use only as the wrapped `func` may dangle.
/// @tparam Func The type of the callable to wrap.
/// @param func The callable to wrap.
template <typename Func>
[[nodiscard]] constexpr auto invoke_wrapper_for(Func &&func) noexcept {

    using decayed_callable_t = std::decay_t<Func>;
    std::is_member_pointer<decayed_callable_t> isMemberPtr{};

    using no_ptr_decayed_callable_t = std::remove_pointer_t<decayed_callable_t>;
    std::is_function<no_ptr_decayed_callable_t> isFunctionPtr{};

    if constexpr (isMemberPtr || isFunctionPtr) {
        return invoke_wrapper<decayed_callable_t>{func};
    } else {

        // Note: a lambda like this should suffice. However:
        //  * MSCV fails to compile if I add a `noexcept` specifier;
        //  * GCC/Clang fails to propagage `noexcept`-ness if I don't add one.
        //
        // auto funcWrapper = [&func](auto &&...args) -> decltype(auto) {
        //     return static_cast<Func &&>(func)(
        //         static_cast<decltype(args)>(args)...);
        // };
        //
        // So, I needed to introduce callable_obj_wrapper<>.

        // If `func` is an object type, I wrap it so that:
        //  * I don't copy nor move `func` unnecessarily;
        //  * I don't require it to be copy/move constructible.
        //
        // However, I'll have a dangling reference if the returned
        // `invoke_wrapper` outlives `func`.
        return invoke_wrapper<callable_obj_wrapper<Func &&>>{
            static_cast<Func &&>(func)};
    }
}

} // namespace detail

struct invoke_t {

#define TR_INVOKE_EXPR(FUNC, ARG_PACK)                                         \
    detail::invoke_wrapper_for(static_cast<decltype(FUNC)>(FUNC))(             \
        static_cast<decltype(ARG_PACK)>(ARG_PACK)...)

    template <typename Func, typename... Args>
    constexpr auto operator()(Func &&func, Args &&...args) const
        noexcept(noexcept(TR_INVOKE_EXPR(func, args)))
            -> decltype(TR_INVOKE_EXPR(func, args)) {
        return TR_INVOKE_EXPR(func, args);
    }

#undef TR_INVOKE_EXPR
};

static constexpr invoke_t invoke{};

} // namespace tr