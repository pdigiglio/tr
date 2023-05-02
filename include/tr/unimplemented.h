#pragma once

#include <type_traits>

namespace tr {

/// @brief Base class for unimplemented tag-dispatched function objects.
struct unimplemented {};

template <typename, typename = void>
struct is_implemented : std::false_type {};

template <typename T>
struct is_implemented<T, std::enable_if_t<!std::is_base_of_v<unimplemented, T>>>
    : std::true_type {};

template <typename T>
static constexpr bool is_implemented_v{is_implemented<T>::value};

} // namespace tr