#pragma once

#include <tr/lazy_false.h>
#include <type_traits>

namespace tr {

/// @brief A callable object to select a free or member function from an
/// overload set.
///
/// @details Trying to instanciate this class template will cause a compilation
/// error. The purpose of this primary template is to catch this kind of errors
/// at compile-time.
///
/// @tparam Signature 
/// @tparam  
template <typename Signature, typename = void>
struct overload_t {
    static_assert(lazy_false<Signature>,
                  "Signature is not a function signature");
};

/// @brief A callable object to select a free or member function from an
/// overload set.
///
/// @details `tr::overload` works on both free functions and member functions:
///
/// @code
/// int f(int);
/// int f(char);
///
/// auto f_ptr0 = tr::overload<int(int)>(f);
/// //   ^ type of f_ptr0 is int(*)(int)
/// 
/// auto f_ptr1 = tr::overload<int(char)>(&f);
/// //                         ok as well ^
///
/// // tr::overload<int(*)(int)>(f);
/// // Not ok: int(*)(int) is not a function type
///
/// struct S {
///   int f(int);
///   int f() const&;
/// };
///
/// auto mem_f_ptr = tr::overload<int() const&>(&S::f);
/// //   ^ type of mem_f_ptr is int(S::*)() const&
/// @endcode
///
/// In order to select a member function overload, you may have to pass in an
/// [abominable function
/// type](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2015/p0172r0.html)
/// (as shown in the code above).
///
/// @tparam Signature The signature of the function to extract.
template <typename Signature>
struct overload_t<Signature, std::enable_if_t<std::is_function_v<Signature>>> {

    /// @brief Select a free function out of an overload set.
    /// @details This overload will be SFINAED-out if `Signature` is an
    /// abominable function type, as `FuncSignature*` would be ill-formed.
    /// 
    /// @tparam FuncSignature The function pointer type, i.e. `Signature`.
    /// @param func The function to select.
    /// @return The selected function.
    template <typename FuncSignature = Signature>
    [[nodiscard]] constexpr auto operator()(FuncSignature *func) const noexcept
        -> FuncSignature * {
        return func;
    }

    /// @brief Select a member function out of an overload set.
    /// @tparam C The type of the class to which the member function belongs.
    /// @param memFunc The member function to select.
    /// @return The selected member function.
    template <typename C>
    [[nodiscard]] constexpr auto
    operator()(Signature C::*memFunc) const noexcept -> Signature C::* {
        return memFunc;
    }
};

template <typename Signature>
static constexpr overload_t<Signature> overload{};

} // namespace tr