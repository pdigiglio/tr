// tr-specific tuple protocol:
//
//  * Specialization of `tup_size`
//  * Specialization of `tup_elem`
//  * Overload of get in `tr` namespace
//

#pragma once

#include "./detail/type_traits.h"
#include "./value_constant.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace tr {
template <typename T>
struct tup_size;

template <typename T>
static constexpr std::size_t tup_size_v{tup_size<T>::value};

template <typename T>
struct tup_size<T const> : tup_size<T> {};

template <std::size_t I, typename T>
struct tup_elem;

template <std::size_t I, typename T>
using tup_elem_t = typename tup_elem<I, T>::type;

template <std::size_t I, typename T>
struct tup_elem<I, T const>
    : detail::type_identity<std::add_const<tup_elem_t<I, T>>> {};

template <auto I, typename T,
          typename = std::enable_if_t<std::is_integral_v<decltype(I)>>>
[[nodiscard]] constexpr decltype(auto) get(T &&tup,
                                           value_constant<I>) noexcept {
    using tr::get, std::get;
    return get<I>(std::forward<T>(tup));
}
} // namespace tr
