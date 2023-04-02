#pragma once

namespace tr {

/// @brief A variable template that depends on a type parameter pack and always
/// evaluates to `false`.
///
/// @details Before [P2539](https://wg21.link/p2593), compilers could reject
/// code like this:
/// 
/// ```cpp
/// template <typename T>
/// void f(T) {
/// 	auto checkT = //...
///		if constexpr (checkT) {
///			// do something
/// 	} else {
///         static_assert(false, "possible bug");
///         //            ^ use lazy_false<T>, instead
///     }
/// }
/// ```
/// 
/// even if the `else` branch is never instanciated. `lazy_false` is a
/// workaround to have the pattern above compile. See also
/// [P1830](https://wg21.link/p1830).
/// 
/// @tparam ... Ts The (unused) pack of type parameters this variable template
/// depends upon.
template <typename... Ts>
constexpr bool lazy_false{false};

} // namespace tr