#pragma once

#include <tr/fwd/type_pack.h>
#include <tr/macros.h>

#include <type_traits>

namespace tr {
namespace detail {

/// @brief A callable wrapper for a function pointer.
///
/// @tparam FPtr The function pointer type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The function pointer return type.
template <typename FPtr, bool NoExcept, typename R, typename /*ArgPack*/ = void>
struct fptr_wrapper {
    FPtr Func_;

    /// @brief Call the target function pointer.
    /// @tparam ...Args The deduced parameter types.
    /// @param ...args The parameters to be forwarded to the target pointer.
    /// @return The target function pointer return value.
    template <typename... Args>
    constexpr auto operator()(Args &&...args) const noexcept(NoExcept) -> R {
        return this->Func_(static_cast<Args &&>(args)...);
    }
};

/// @brief A callable wrapper for a function pointer.
///
/// @tparam FPtr The function pointer type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The function pointer return type.
/// @tparam ...Args The function pointer argument types.
template <typename FPtr, bool NoExcept, typename R, typename... Args>
struct fptr_wrapper<FPtr, NoExcept, R, type_pack<Args...>> {
    FPtr Func_;

    /// @brief Call the target function pointer.
    /// @param ...args The arguments to the function call.
    /// @return The return value of the target function pointer.
    ///
    /// @note If the target function takes any of its parameters by value, by
    /// calling this wrapper, you'll pay for an extra move (or copy) ctor to
    /// construct the target-function parameter.
    constexpr auto operator()(Args... args) const noexcept(NoExcept) -> R {
        return this->Func_(static_cast<Args &&>(args)...);
        //                             ^
        // if Args is: * value type  -> r-value ref
        //             * r-value ref -> r-value ref
        //             * l-value ref -> l-value ref
    }
};

/// @brief A callable wrapper for a pointer to member function.
///
/// @tparam Derived The derived type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The member function pointer return type.
/// @tparam C The type of the class to call the member function pointer on
/// (must be a reference type).
template <typename Derived, bool NoExcept, typename R, typename C,
          typename /*ArgPack*/ = void>
struct mem_fptr_wrapper_crtp {
    static_assert(std::is_reference_v<C>, "C should be a reference type");

    /// @brief Call the target member function pointer.
    /// @tparam ...Args The deduced parameter types.
    /// @param ...args The parameters to be forwarded to the target pointer.
    /// @return The target member function pointer return value.
    template <typename... Args>
    constexpr auto operator()(C c, Args &&...args) const noexcept(NoExcept)
        -> R {
        auto func = static_cast<Derived const &>(*this).Func_;
        return (static_cast<C>(c).*func)(static_cast<Args &&>(args)...);
    }
};

/// @brief A callable wrapper for a pointer to member function.
///
/// @tparam Derived The derived type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The member function pointer return type.
/// @tparam C The type of the class to call the member function pointer on
/// (must be a reference type).
/// @tparam ...Args The member function pointer argument types.
template <typename Derived, bool NoExcept, typename R, typename C,
          typename... Args>
struct mem_fptr_wrapper_crtp<Derived, NoExcept, R, C, type_pack<Args...>> {
    static_assert(std::is_reference_v<C>, "C should be a reference type");

    /// @brief Call the target member function pointer.
    /// @param ...args The arguments to the function call.
    /// @return The return value of the target member function pointer.
    ///
    /// @note If the target function takes any of its parameters by value, by
    /// calling this wrapper, you'll pay for an extra move (or copy) ctor to
    /// construct the target-function parameter.
    constexpr auto operator()(C c, Args... args) const noexcept(NoExcept) -> R {
        auto func = static_cast<Derived const &>(*this).Func_;
        return (static_cast<C>(c).*func)(static_cast<Args &&>(args)...);
        //                                           ^
        // if Args is: * value type  -> r-value ref
        //             * r-value ref -> r-value ref
        //             * l-value ref -> l-value ref
    }
};

/// @brief A callable wrapper for a pointer to member function (`C*`
/// specialization).
///
/// @tparam Derived The derived type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The member function pointer return type.
/// @tparam C The type of the class to call the member function pointer on.
template <typename Derived, bool NoExcept, typename R, typename C>
struct mem_fptr_wrapper_crtp<Derived, NoExcept, R, C *, void> {

    /// @brief Call the target member function pointer.
    /// @tparam ...Args The deduced parameter types.
    /// @param c The class instance to call the member function pointer on.
    /// @param ...args The parameters to be forwarded to the target pointer.
    /// @return The target member function pointer return value.
    template <typename... Args>
    constexpr auto operator()(C *c, Args &&...args) const noexcept(NoExcept)
        -> R {
        auto func = static_cast<Derived const &>(*this).Func_;
        return (c->*func)(static_cast<Args &&>(args)...);
    }
};

/// @brief A callable wrapper for a pointer to member function (`C*`
/// specialization).
///
/// @tparam Derived The derived type.
/// @tparam NoExcept Whether the function pointer is `noexcept`.
/// @tparam R The member function pointer return type.
/// @tparam C The type of the class to call the member function pointer on.
/// @tparam ...Args The member function pointer argument types.
template <typename Derived, bool NoExcept, typename R, typename C,
          typename... Args>
struct mem_fptr_wrapper_crtp<Derived, NoExcept, R, C *, type_pack<Args...>> {

    /// @brief Call the target member function pointer.
    /// @param c The class instance to call the member function pointer on.
    /// @param ...args The parameters to be forwarded to the target pointer.
    /// @return The target member function pointer return value.
    ///
    /// @note If the target function takes any of its parameters by value, by
    /// calling this wrapper, you'll pay for an extra move (or copy) ctor to
    /// construct the target-function parameter.
    constexpr auto operator()(C *c, Args... args) const noexcept(NoExcept)
        -> R {
        auto func = static_cast<Derived const &>(*this).Func_;
        return (c->*func)(static_cast<Args &&>(args)...);
        //                            ^
        // if Args is: * value type  -> r-value ref
        //             * r-value ref -> r-value ref
        //             * l-value ref -> l-value ref
    }
};

template <typename Derived, bool NoExcept, typename Overloads, typename R,
          typename ArgPack = void>
struct mem_fptr_wrapper;

template <typename MemFPtr, bool NoExcept, typename... Cs, typename R,
          typename ArgPack>
struct TR_EMPTY_BASES
    mem_fptr_wrapper<MemFPtr, NoExcept, type_pack<Cs...>, R, ArgPack>
    : mem_fptr_wrapper_crtp<
          mem_fptr_wrapper<MemFPtr, NoExcept, type_pack<Cs...>, R, ArgPack>,
          NoExcept, R, Cs, ArgPack>... {

    using mem_fptr_wrapper_crtp<
        mem_fptr_wrapper<MemFPtr, NoExcept, type_pack<Cs...>, R, ArgPack>,
        NoExcept, R, Cs, ArgPack>::operator()...;

    constexpr mem_fptr_wrapper(MemFPtr func) noexcept : Func_(func) {}
    MemFPtr Func_;
};

template <typename MemPtr>
struct mem_ptr_wrapper;

/// @brief A callable wrapper for a pointer to member variable.
///
/// @tparam R The member variable type.
/// @tparam C The type of the class the member variable belongs to.
template <typename R, typename C>
struct mem_ptr_wrapper<R C::*> {
    R C::*MemPtr_;

    constexpr auto operator()(C &c) const noexcept -> R & {
        return (c.*this->MemPtr_);
    }

    constexpr auto operator()(C const &c) const noexcept -> R const & {
        return (c.*this->MemPtr_);
    }

    constexpr auto operator()(C &&c) const noexcept -> R && {
        return (static_cast<C &&>(c).*this->MemPtr_);
    }

    constexpr auto operator()(C const &&c) const noexcept -> R const && {
        return (static_cast<C const &&>(c).*this->MemPtr_);
    }

    constexpr auto operator()(C *c) const noexcept -> R & {
        return (c->*this->MemPtr_);
    }

    constexpr auto operator()(C const *c) const noexcept -> R const & {
        return (c->*this->MemPtr_);
    }
};

} // namespace detail
} // namespace tr